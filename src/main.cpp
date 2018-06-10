#include "hash_functions.h"
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<limits.h>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<set>
#include<map>
#include<cmath>
using namespace std;

#define STR_MAX_LEN_INPUT 8
#define STR_MAX_LEN 4
#define INPUT_NUM 10000

struct MyInput {
public:
	char key[STR_MAX_LEN_INPUT];
	MyInput(){
		memset(key, 0, STR_MAX_LEN_INPUT);
	}
};

class MyKey {
public:
	char key[STR_MAX_LEN];
	MyKey(){
		memset(key, 0, STR_MAX_LEN);
	}
};

bool operator < (MyKey an, MyKey bn) {
	for (int i = 0;i < STR_MAX_LEN;i++) {
		if (bn.key[i] < an.key[i]) {
			return true;
		}
		else if (bn.key[i] > an.key[i]) {
			return false;
		}
	}
	return false;
}

bool operator == (MyKey an, MyKey bn) {
	for (int i = 0;i < STR_MAX_LEN;i++) {
		if (bn.key[i] != an.key[i]) {
			return false;
		}
	}
	return true;
}


class CMSketch {
public:
	int memory_in_bytes;
	int kCountersNum_;
	int hash_number;
	int hash_seed_offset;
	int *counters_;
	int num_row_counters_;

	CMSketch(int memory_in_bytes_, int hash_number_, int hash_seed_offset_) {
		memory_in_bytes = memory_in_bytes_;
		kCountersNum_ = memory_in_bytes * 8 / 32;
		hash_number = hash_number_;
		hash_seed_offset = hash_seed_offset_;
		counters_ = new int[kCountersNum_];
		memset(counters_, 0, kCountersNum_ * sizeof(int));
		num_row_counters_ = kCountersNum_ / hash_number;
	}

	void insert(const char* key) {
		for (int i = 0; i < hash_number;i++) {
			unsigned int index = murmur3_32(key, STR_MAX_LEN, i + hash_seed_offset) % num_row_counters_;
			counters_[i * num_row_counters_ + index]++;
		}
	}

	int query(const char* key) {
		int ret = INT_MAX;
		for (int i = 0; i < hash_number;i++) {
			int tmp = counters_[i * num_row_counters_ + murmur3_32(key, STR_MAX_LEN, i + hash_seed_offset) % num_row_counters_];
			if (tmp < ret) {
				ret = tmp;
			}
		}
		return ret;
	}

};


class CUSketch {
public:
	int memory_in_bytes;
	int kCountersNum_;
	int hash_number;
	int hash_seed_offset;
	int *counters_;
	int num_row_counters_;

	CUSketch(int memory_in_bytes_, int hash_number_, int hash_seed_offset_) {
		memory_in_bytes = memory_in_bytes_;
		kCountersNum_ = memory_in_bytes * 8 / 32;
		hash_number = hash_number_;
		hash_seed_offset = hash_seed_offset_;
		counters_ = new int[kCountersNum_];
		memset(counters_, 0, kCountersNum_ * sizeof(int));
		num_row_counters_ = kCountersNum_ / hash_number;
	}

	void insert(const char* key) {
		int ret = INT_MAX;
		for (int i = 0; i < hash_number;i++) {
			unsigned int index = murmur3_32(key, STR_MAX_LEN, i + hash_seed_offset) % num_row_counters_;
			if (counters_[i * num_row_counters_ + index] < ret) {
				ret = counters_[i * num_row_counters_ + index];
			}
		}
		for (int i = 0; i < hash_number;i++) {
			unsigned int index = murmur3_32(key, STR_MAX_LEN, i + hash_seed_offset) % num_row_counters_;
			if (counters_[i * num_row_counters_ + index] == ret) {
				counters_[i * num_row_counters_ + index]++;
			}
		}
	}

	int query(const char* key) {
		int ret = INT_MAX;
		for (int i = 0; i < hash_number;i++) {
			int tmp = counters_[i * num_row_counters_ + murmur3_32(key, STR_MAX_LEN, i + hash_seed_offset) % num_row_counters_];
			if (tmp < ret) {
				ret = tmp;
			}
		}
		return ret;
	}
};


class NewCUSketch {
public:
	int memory_in_bytes;
	int *pMemory;
	int hash_number;
	int hash_seed_offset;
	int kCountersNum_;
	int *counters_;


	NewCUSketch(int memory_in_bytes_, int *pMemory_, int hash_number_, int hash_seed_offset_) {
		memory_in_bytes = memory_in_bytes_;
		kCountersNum_ = memory_in_bytes * 8 / 32;
		counters_ = new int[kCountersNum_];
		memset(counters_, 0, kCountersNum_ * sizeof(int));
		pMemory = new int[hash_number_];
		memcpy(pMemory, pMemory_, hash_number_ * sizeof(int));
		hash_number = hash_number_;
		hash_seed_offset = hash_seed_offset_;
	}

	void insert(const char* key) {
		int ret = counters_[murmur3_32(key, STR_MAX_LEN, 0 + hash_seed_offset) % (pMemory[0] / 4)];
		ret++;
		counters_[murmur3_32(key, STR_MAX_LEN, 0 + hash_seed_offset) % (pMemory[0] / 4)]++;
		for (int i = 1; i < hash_number;i++) {
			int index_offset = 0;
			for (int j = 0; j < i;j++) {
				index_offset = index_offset + (pMemory[j] / 4);
			}
			unsigned int index = murmur3_32(key, STR_MAX_LEN, i + hash_seed_offset) % (pMemory[i] / 4);
			if (counters_[index_offset + index] < ret) {
				counters_[index_offset + index] ++;
				ret = counters_[index_offset + index];
			}
		}
	}

	int query(const char* key) {
		int ret = INT_MAX;
		for (int i = 0; i < hash_number;i++) {
			int index_offset = 0;
			for (int j = 0; j < i;j++) {
				index_offset = index_offset + (pMemory[j] / 4);
			}
			int tmp = counters_[index_offset + murmur3_32(key, STR_MAX_LEN, i + hash_seed_offset) % (pMemory[i] / 4)];
			if (tmp < ret) {
				ret = tmp;
			}
		}
		return ret;
	}
};

MyInput my_input[INPUT_NUM];
MyKey my_key[INPUT_NUM];

//1.memory_in_bytes/10000 2.hash_numbers 3.memory_ratio 4.out_put_model 5.file_name 6.read_file_name
int main(int argc, char* argv[]) {
	int memory_in_bytes = atoi(argv[1]);
	memory_in_bytes = memory_in_bytes * 10000;
	const int hash_number = atoi(argv[2]);
	double ratio = atof(argv[3]);
	int out_model = atoi(argv[4]);
	int *memory_of_row = new int[hash_number];
	if (fabs(ratio - 1) < 0.00001) {
		for (int i = 0; i < hash_number;i++) {
			memory_of_row[i] = memory_in_bytes / hash_number;
		}
		memory_of_row[hash_number - 1] = memory_in_bytes;
		for (int i = 0; i < hash_number - 1; i++) {
			memory_of_row[hash_number - 1] = memory_of_row[hash_number - 1] - memory_of_row[i];
		}
	}
	else {
		memory_of_row[0] = memory_in_bytes * (ratio / (ratio + (double)hash_number - 1));
		for (int i = 1;i < hash_number;i++) {
			memory_of_row[i] = memory_in_bytes / (ratio + (double)hash_number - 1);
		}
		memory_of_row[hash_number - 1] = memory_in_bytes;
		for (int i = 0; i < hash_number - 1; i++) {
			memory_of_row[hash_number - 1] = memory_of_row[hash_number - 1] - memory_of_row[i];
		}
	}


	CMSketch cm_sketch(memory_in_bytes, hash_number, 2);
	memset(cm_sketch.counters_, 0, memory_in_bytes);
	CUSketch cu_sketch(memory_in_bytes, hash_number, 2);
	memset(cu_sketch.counters_, 0, memory_in_bytes);
	NewCUSketch new_cu_sketch(memory_in_bytes, memory_of_row, hash_number, 2);
	memset(new_cu_sketch.counters_, 0, memory_in_bytes);
	set<MyKey> my_set;
	map<MyKey, int> my_map;


	fstream fin(argv[6], ios::in | ios::binary);
	ofstream fout;
	fout.open(argv[5], ios::app);
	for (int i = 0; i < INPUT_NUM;i++) {
		fin.read((char*)(&my_input[i]), STR_MAX_LEN_INPUT);
	}
	fin.close();
	for (int i = 0; i < INPUT_NUM;i++) {
		memcpy(my_key[i].key, my_input[i].key, STR_MAX_LEN);
		cm_sketch.insert(my_key[i].key);
		cu_sketch.insert(my_key[i].key);
		new_cu_sketch.insert(my_key[i].key);
		if (my_set.find(my_key[i]) == my_set.end()) {
			my_set.insert(my_key[i]);
		}
		my_map[my_key[i]] += 1;
	}

	map<MyKey, int>::iterator iter;

	double cm_aae = 0;
	double cu_aae = 0;
	double new_cu_aae = 0;

	double cm_are = 0;
	double cu_are = 0;
	double new_cu_are = 0;

	double cm_cr = 0;
	double cu_cr = 0;
	double new_cu_cr = 0;
	for (iter = my_map.begin();iter != my_map.end();iter++) {
		cm_aae = cm_aae + cm_sketch.query(iter->first.key) - iter->second;
		cu_aae = cu_aae + cu_sketch.query(iter->first.key) - iter->second;
		new_cu_aae = new_cu_aae + new_cu_sketch.query(iter->first.key) - iter->second;

		cm_are = cm_are + (double)(cm_sketch.query(iter->first.key) - iter->second) / (double)iter->second;
		cu_are = cu_are + (double)(cu_sketch.query(iter->first.key) - iter->second) / (double)iter->second;
		new_cu_are = new_cu_are + (double)(new_cu_sketch.query(iter->first.key) - iter->second) / (double)iter->second;

		if (cm_sketch.query(iter->first.key) == iter->second) {
			cm_cr = cm_cr + 1;
		}
		if (cu_sketch.query(iter->first.key) == iter->second) {
			cu_cr = cu_cr + 1;
		}
		if (new_cu_sketch.query(iter->first.key) == iter->second) {
			new_cu_cr = new_cu_cr + 1;
		}
	}
	cm_aae = (double)cm_aae / (double)my_map.size();
	cu_aae = (double)cu_aae / (double)my_map.size();
	new_cu_aae = (double)new_cu_aae / (double)my_map.size();

	cm_are = cm_are / (double)my_map.size();
	cu_are = cu_are / (double)my_map.size();
	new_cu_are = new_cu_are / (double)my_map.size();

	cm_cr = cm_cr / (double)my_map.size();
	cu_cr = cu_cr / (double)my_map.size();
	new_cu_cr = new_cu_cr / (double)my_map.size();

	switch (out_model) {
	case 0:
		fout << "," << log(cm_aae);
		break;
	case 1:
		fout << "," << log(cu_aae);
		break;
	case 2:
		fout << "," << log(new_cu_aae);
		break;
	case 3:
		fout << "," << log(cm_are);
		break;
	case 4:
		fout << "," << log(cu_are);
		break;
	case 5:
		fout << "," << log(new_cu_are);
		break;
	case 6:
		fout << "," << cm_cr;
		break;
	case 7:
		fout << "," << cu_cr;
		break;
	case 8:
		fout << "," << new_cu_cr;
		break;
	}
	
}

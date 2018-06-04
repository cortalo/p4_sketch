# p4_sketch

argv[1]:The memory of Sketch, in 10000 Bytes. (For example, 5 means 50000 Bytes)

argv[2]:The number of hash functions

argv[3]:The ratio of the first level memory to the second level memory

argv[4]:output model.
  
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
 
argv[5]:output file name

argv[6]:read in file name

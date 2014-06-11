#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "file_op.h"
#include "common.h"

namespace bible{
	using namespace std;
	void ensureFileExists(const char* filename) {
		if (getFileLength(filename)<=0) {
			createEmptyFile(filename);
		}
	}
	void createEmptyFile(const char* filename) {
		FILE* tmp=fopen(filename,"w");
		fclose(tmp);
	}

	///< attentio: add \0 to the end of the file.
	void loadFile(const char * filename, char * &memblock, size_t &filesize){
		memblock = NULL;
		size_t size;
		ifstream file (filename, ios::in|ios::binary|ios::ate);

		if (file.is_open()) {
			size = file.tellg();
			filesize = size;
			memblock = new char[size + 1];
			file.seekg(0, ios::beg);
			file.read(memblock, size);
			file.close();
			debug_print("read file %s content.", filename);
		}
		else{
			cout << "Unable to open file";
			exit(0);
		}	
	}

	int getFileLength(const char* filename) {
		struct stat buf;
		if (stat(filename, &buf) == -1) {
			return 0;
		}
		else{
			return buf.st_size;
		}
	}
} // end namespace bible.

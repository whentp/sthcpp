#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "file_op.h"
#include "common.h"

//#ifdef __posix
#include <dirent.h>
//#else
//#include "dirent.h"
//#endif

namespace bible{
	using namespace std;
	void ensureFileExists(const char* filename) {
		if (getFileLength(filename)<=0) {
			createEmptyFile(filename);
		}
	}
	void createEmptyFile(const char* filename) {
		FILE* tmp = fopen(filename,"w");
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

	size_t getFileLength(const char* filename) {
		struct stat buf;
		if (stat(filename, &buf) == -1) {
			return 0;
		}
		else{
			return buf.st_size;
		}
	}

	vector<string> *getFilesInDirectory(const char *directory) {
		DIR *dir;
		struct dirent *ent;
		struct stat eStat;

		auto res = new vector<string>;
		if ((dir = opendir(directory)) != NULL) {
			while ((ent = readdir (dir)) != NULL) {
				lstat(ent->d_name, &eStat);
				//cout << ent->d_name << "\t" << S_ISDIR(eStat.st_mode)<<endl;
				//if(!S_ISDIR(eStat.st_mode)){
				res->push_back(ent->d_name);
				//}
			}
			closedir (dir);
		}
		return res;
	}

} // end namespace bible.

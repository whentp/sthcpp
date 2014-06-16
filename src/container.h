#ifndef _CONTAINER_H__
#define _CONTAINER_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "barn.h"
#include "structure.h"

using namespace std;

namespace bible{

/**
 * @brief An ugly wrapper for barn.
 */
 class Container{
 private:
 	string _filename_keys;
 	string _filename_values;
 	Barn _barn;
 public:
 	
 	Container(const char *single_file_name);
 	Container(){};
 	~Container();

 	void Open(const char *single_file_name);

 	BibleIntType GetFileNumber(const char *filename);

 	const char *GetFilename(BibleIntType index);

 	size_t Merge(const char *single_file_name);

 	void Close();
 };


} // end namespace bible.
#endif

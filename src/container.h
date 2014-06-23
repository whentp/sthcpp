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

 	/**
 	 * @brief return an id for a given string. this method is just for storing STRING.
 	 * pls specify length when u want to save binary stuff.
 	 * 
 	 * @param[in] filename the given string
 	 * 
 	 * @return the id
 	 */
 	BibleIntType GetFileNumber(const char *filename);
 	
 	/**
 	 * @brief return an id for a given block
 	 * 
 	 * @param[in] filename the given block
 	 * @param[in] length length of the given block
 	 * 
 	 * @return the id
 	 */
 	BibleIntType GetFileNumber(const char *filename, size_t length);

 	const char *GetFilename(BibleIntType index);
 	const char *GetFilename(BibleIntType index, size_t &length); ///< attention: length is a parameter for output.

 	size_t Merge(const char *single_file_name);

 	void Close();
 };


} // end namespace bible.
#endif

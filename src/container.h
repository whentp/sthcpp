#ifndef _CONTAINER_H__
#define _CONTAINER_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "barn.h"

using namespace std;

namespace bible{

	class Container{
	private:
		string _filename_keys;
		string _filename_values;
		Barn _barn;
	public:
	//Container(
	//	const char *filename_keys,
	//	const char *filename_container): _filename_keys(filename_keys), _filename_values(filename_values){};

		Container(const char *single_file_name);
		Container(){};
		~Container();

		void Open(const char *single_file_name);

		unsigned int GetFileNumber(const char *filename);

		const char *GetFilename(size_t index);

		size_t Merge(const char *single_file_name);

		void Close();
	};


} // end namespace bible.
#endif

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "common.h"
#include "barn.h"
#include "container.h"
#include "structure.h"

using namespace std;

namespace bible{

	Container::~Container(){
		Close();
	}

	Container::Container(const char *single_file_name){
		Open(single_file_name);
	}

	void Container::Open(const char *single_file_name){
		_filename_keys = single_file_name;
		_filename_keys += file_ext_container_key;
		_filename_values = single_file_name;
		_filename_values += file_ext_container_value;
		_barn.Open(_filename_keys.c_str(), _filename_values.c_str());
	}

	BibleIntType Container::GetFileNumber(const char *filename){
	//Barn tmpbarn(_filename_keys.c_str(), _filename_values.c_str());
		string tmpstr = filename;
		return _barn.Set(tmpstr.c_str(), tmpstr.size() + 1);
	}

	BibleIntType Container::GetFileNumber(const char *filename, size_t length){
		return _barn.Set(filename, length);
	}

	const char *Container::GetFilename(BibleIntType index){
		size_t i;
		char *a;
		_barn.Get(index, a, i);
		//cout << "finding filename:" << index  << a << endl;
		return a;
	}

	const char *Container::GetFilename(BibleIntType index, size_t &length){
		char *a;
		_barn.Get(index, a, length);
		//cout << "finding filename:" << index  << a << endl;
		return a;
	}

	size_t Container::Merge(const char *single_file_name){
	//Barn tmpbarn(_filename_keys.c_str(), _filename_values.c_str());
		string tmp = "";
		return _barn.Merge(
			(tmp + single_file_name + file_ext_container_key).c_str(),
			(tmp + single_file_name + file_ext_container_value).c_str());
	}

	void Container::Close(){
		_barn.Close();
	// nothing.
	}

} // end namespace bible.

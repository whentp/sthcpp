#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "common.h"
#include "barn.h"
#include "container.h"

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
		_filename_keys += ".container";
		_filename_values = single_file_name;
		_filename_values += ".barnvalue";

		_barn.Open(_filename_keys.c_str(), _filename_values.c_str());
	}

	unsigned int Container::GetFileNumber(const char *filename){
	//Barn tmpbarn(_filename_keys.c_str(), _filename_values.c_str());
		string tmpstr = filename;
		return _barn.Set(tmpstr.c_str(), tmpstr.size() + 1);
	}

	const char *Container::GetFilename(size_t index){
		size_t i;
		char *a;
		_barn.Get(index, a, i);
		//cout << "finding filename:" << index  << a << endl;
		return a;
	}

	size_t Container::Merge(const char *single_file_name){
	//Barn tmpbarn(_filename_keys.c_str(), _filename_values.c_str());
		string tmp = "";
		return _barn.Merge(
			(tmp + single_file_name + ".container").c_str(),
			(tmp + single_file_name + ".barnvalue").c_str());
	}

	void Container::Close(){
		_barn.Close();
	// nothing.
	}

} // end namespace bible.
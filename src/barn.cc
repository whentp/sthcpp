#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "common.h"
#include "barn.h"
#include "file_op.h"

using namespace std;

namespace bible{

	Barn::~Barn(){
		Close();
	}

	void Barn::Close(){
		_file_keys.close();
		_file_values.close();
		if(_mem){
			delete[] _mem;
		}
	}

	void Barn::Open(
		const char *filename_keys,
		const char *filename_values,
		BarnOpenMode mode)
	{
		ensureFileExists(filename_keys);
		_file_keys.open(filename_keys, ios::in | ios::out | ios::app | ios::binary | ios::ate);
		if(!_file_keys.is_open()){
			cout << "open barn key file error. " << filename_keys << endl;
			exit(0);
		}
		_file_keys_length = _file_keys.tellg();
		ensureFileExists(filename_values);
		_file_values.open(filename_values, ios::in | ios::out | ios::app | ios::binary | ios::ate);
		if(!_file_values.is_open()){
			cout << "open barn value file error. " << filename_values << endl;
			exit(0);
		}
		_file_values_length = _file_values.tellg();
	}

	void Barn::Open(
		const char *filename_keys,
		const char *filename_values)
	{
		Open(filename_keys, filename_values, BOM_GREEDY);
	}

	Barn::Barn(
		const char *filename_keys,
		const char *filename_values,
		BarnOpenMode mode)
	{
		Open(filename_keys, filename_values, mode);
	}

	size_t Barn::Set(const char* content, unsigned int length){
		BarnNode tmpnode;
		_file_values.seekp(0, ios::end);
		tmpnode.start = _file_values.tellp();
		_file_values.write(content, length);
		_file_values_length = _file_values.tellp();

		tmpnode.length = length;
		//cout << "start:" << tmpnode.start << "\tlength:"<<tmpnode.length<<endl;
		_file_keys.seekp(0, ios::end);
		size_t keylength = _file_keys.tellp();
		_file_keys.write((char*)&tmpnode, sizeof(BarnNode));
		_file_keys_length = _file_keys.tellp();
		return keylength / sizeof(BarnNode);
	}

	void Barn::Get(size_t index, char * &to, size_t &length){
		if(index < _file_keys_length / sizeof(BarnNode)){
			BarnNode tmpnode;
			index = index * sizeof(BarnNode);
			_file_keys.seekg(index, ios::beg);
			_file_keys.read((char*)(&tmpnode), sizeof(BarnNode));
			//cout << "read index:" << index 
			//<< " start:" << tmpnode.start 
			//<< "\tlength:"<<tmpnode.length<<endl;

			length = tmpnode.length;
			to = new char[tmpnode.length];
			_file_values.seekg(tmpnode.start, ios::beg);
			_file_values.read(to, tmpnode.length);
		} else {
			cout << "Index out of range. index: " << index << "/" << _file_keys_length << endl;
			exit(0);
		}
	}

	size_t Barn::Merge(
		const char *filename_keys,
		const char *filename_values)
	{
		size_t current_length = _file_keys_length;
		fstream file;
		file.open(filename_keys, ios::in|ios::binary|ios::ate);
		if(!file.is_open()){
			cout << "open barn key file error." << endl;
			exit(0);
		}
		size_t tmplen = file.tellg();
		file.seekg(ios::beg);
		char *tmp = new char[tmplen];
		file.read(tmp, tmplen);
		file.close();
		BarnNode *tmpnode = (BarnNode*)tmp;
		size_t i = tmplen / sizeof(BarnNode);
		while(i-- > 0){
			tmpnode->start += _file_values_length;
			++tmpnode;
		}
		_file_keys.seekg(ios::end);
		_file_keys.write(tmp, tmplen);
		delete[] tmp;
		_file_keys_length = _file_keys.tellg();

		file.open(filename_values, ios::in|ios::binary|ios::ate);
		if(!file.is_open()){
			cout << "open barn value file error. " << filename_values << endl;
			exit(0);
		}
		tmplen = file.tellg();
		file.seekg(ios_base::beg);
		tmp = new char[tmplen];
		file.read(tmp, tmplen);
		file.close();
		_file_values.seekg(ios_base::end);
		_file_values.write(tmp, tmplen);
		_file_values_length = _file_values.tellg();
		delete[] tmp;
		return current_length / sizeof(BarnNode);
	}

} // end namespace bible.

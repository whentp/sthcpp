#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string.h>
#include "tokenizer.h"
#include "tokenizer_english.h"
#include "structure.h"
#include "file_op.h"
#include "common.h"
#include "container.h"

namespace bible{
	using namespace std;

	void addTextToIndex(
		const char *keystr,
		const char *valuestr,
		const char *to,
		const char *fcontainer)
	{
		const char * raw_string = valuestr;
		size_t length = 0;

		Container tmpcontainer(fcontainer);

		unsigned int max_file_offset = getfileoffset(0xffffffff);
		unsigned int max_file_number = getfilenumber(0xffffffff);

		unsigned int file_number = tmpcontainer.GetFileNumber(keystr);

		if (file_number >= max_file_number){
			cout << "container full. cannot add file." << endl;
			exit(0);
		}
		//cout << file_number << endl;
		ensureFileExists(to);

		length = strlen(raw_string);

		auto tokenizer = new TokenizerEnglish();
		auto hashlist = tokenizer->Tokenize(raw_string);

		unsigned int processed_token_count = 0;

		for (auto item = hashlist->begin(); item != hashlist->end(); ++item) {
			// donno how to convert item to void*. therefore write an ugly line.
			TokenItem* current_item = &(*item);
			current_item->offset = makeFileNode(file_number, current_item->offset);
			++processed_token_count;
			if(processed_token_count >= max_file_offset){
				cout << "Only the first " 
				<< processed_token_count << "tokens are indexed." << endl;
				break;
			}
		}

		if (hashlist->size()){
			fstream file (to, ios::in|ios::out|ios::binary|ios::ate);
			if (file.is_open()) {
				file.write((const char *)(&((*hashlist)[0])), sizeof(IndexNode) * processed_token_count);
				file.close();
				cout << "Processing " << keystr 
				<< ", length: " << length << " bytes, ratio: " 
				<< (double)processed_token_count * sizeof(IndexNode) / length << endl;
			}
		}
		delete hashlist;
		//any leak?
	}

	void addFileToIndex(
		const char * filename,
		const char * to,
		const char * fcontainer)
	{
		char * raw_string = NULL;
		size_t filelength = 0;
		loadFile(filename, raw_string, filelength);
		addTextToIndex(filename, raw_string, to, fcontainer);
		delete[] raw_string;
	}

	int sortIndex(const char * filename) {
		int filelength = getFileLength(filename);
		int tmpint = filelength / sizeof(CompareNode);
		FILE* filenameindex = fopen(filename,"rb+"); // errr... i use c here just because i donno the alternative for "rb+" in c++.
		auto tmp = new CompareNode[tmpint];

		printf("Sort index start...\n");
		fseek(filenameindex, 0, SEEK_SET);
		rewind(filenameindex);
		fread(tmp, sizeof(CompareNode), tmpint, filenameindex);
		sort(tmp, tmp + tmpint, compareIndex);
		fseek(filenameindex, 0, SEEK_SET);
		rewind(filenameindex);
		fwrite(tmp, sizeof(CompareNode), tmpint, filenameindex);
		fclose(filenameindex);
		printf("Sort index OK...\n");

		delete[] tmp;
		return tmpint;
	}

	int compressIndex(
		const char * filename_raw,
		const char * filename_compress,
		const char * filename_keyindex)
	{
		int filelength = getFileLength(filename_raw);
		int tmpint = filelength / sizeof(CompareNode);
		int currentp = 0, m1;
		FILE* filenameindex;
		FILE* compressf;
		FILE* keyf;
		KeyNode tmpkeynode;
		TokenItem *tmp = new TokenItem[tmpint];
		filenameindex = fopen(filename_raw,"rb");
		fseek(filenameindex,0,SEEK_SET);
		fread(tmp,1,filelength,filenameindex);
		fclose(filenameindex);
		compressf=fopen(filename_compress,"wb");
		rewind(compressf);
		keyf = fopen(filename_keyindex,"wb");
		tmpkeynode.key = 0;
		tmpkeynode.start = 0;
		tmpkeynode.length = 0;
		for(int i = 0; i < tmpint; ++i) {
			if(tmpkeynode.key != tmp[i].hash) {
				m1=ftell(compressf);
				tmpkeynode.length=m1-currentp;
				fwrite(&tmpkeynode,sizeof(tmpkeynode),1,keyf);
				tmpkeynode.key=tmp[i].hash;
				tmpkeynode.start=m1;
				currentp=m1;
			}
			fwrite(&(tmp[i].offset),sizeof(int),1,compressf);
		}
		m1=ftell(compressf);
		tmpkeynode.length=m1-currentp;
		fwrite(&tmpkeynode,sizeof(tmpkeynode),1,keyf);
		fclose(compressf);
		fclose(keyf);
		delete[] tmp;
		remove(filename_raw);
		printf("Compress index OK...\n");
		return tmpint;
	}

	void mergeIndex(
		const char * container1,
		const char * container2,
		const char * tmpcontainer,
		const char * keyindex1,
		const char * keyindex2,
		const char * tmpkeyindex,
		const char * compressed1,
		const char * compressed2,
		const char * tmpcompressed)
	{
		cout << container1 << endl;
		cout << container2 << endl;
		cout << keyindex1 << endl;
		cout << keyindex2 << endl;
		cout << compressed1 << endl;
		cout << compressed2 << endl;

		size_t len1 = getFileLength(container1);
		size_t len2 = getFileLength(container2);

		// merge two container. simple.
		
		Container container(container1);

		len1 = container.Merge(container2);
		container.Close();

		string tmp1 = file_ext_container_key;//".container";
		rename((container1 + tmp1).c_str(), (tmpcontainer + tmp1).c_str());
		string tmp2 = file_ext_container_value; //".barnvalue";
		rename((container1 + tmp2).c_str(), (tmpcontainer + tmp2).c_str());

		unsigned int file_number_offset = makeFileNode(len1, 0);
		// a little dangder. generate an offset used for calculate new file numbers.

		fstream newkey(tmpkeyindex, ios::out|ios::binary);
		fstream newcompressed(tmpcompressed, ios::out|ios::binary);
		len1 = getFileLength(keyindex1);
		len2 = getFileLength(keyindex2);
		fstream fk1(keyindex1, ios::in|ios::binary);
		fstream fk2(keyindex2, ios::in|ios::binary);
		fstream fc1(compressed1, ios::in|ios::binary);
		fstream fc2(compressed2, ios::in|ios::binary);
		if(!fk1.is_open()) throw "error";
		if(!fk2.is_open()) throw "error";
		if(!fc1.is_open()) throw "error";
		if(!fc2.is_open()) throw "error";

		size_t i = 0, j = 0;

		KeyNode tmpk1, tmpk2;
		//hope all keys can be read to memory. 
		fk1.read((char*)&tmpk1, sizeof(KeyNode));
		fk2.read((char*)&tmpk2, sizeof(KeyNode));

		char *b1, *b2;

		len1 /= sizeof(KeyNode);
		len2 /= sizeof(KeyNode);
		while(i < len1 || j < len2){
			//cout << "i="<< i << "/" << len1 << "\tj="<< j << "/" << len2 << endl;
			if(tmpk1.key < tmpk2.key){
				if (tmpk1.length > 0) {
					b1 = new char[tmpk1.length];
					fc1.read(b1, tmpk1.length);
					tmpk1.start = newcompressed.tellg();
					newcompressed.write(b1, tmpk1.length);
					newkey.write((char*)&tmpk1, sizeof(KeyNode));
					delete[] b1;
				}
				++i;
				if(i < len1){
					fk1.read((char*)&tmpk1, sizeof(KeyNode));
				} else {
					tmpk1.key = 0xffffffff;//-1; // is it safe? or UINT_MAX?
				}
			} else if(tmpk1.key > tmpk2.key){
				if (tmpk2.length > 0) {
					b2 = new char[tmpk2.length];
					fc2.read(b2, tmpk2.length);

					// the following section... urrrr... how can remove it when we do not need it.
					// e.g. when every file_number is already unique.
					int count = tmpk2.length / sizeof(int);
					unsigned int *tmpadder = (unsigned int *)(b2);
					while(count-- > 0){
						(*tmpadder) += file_number_offset;
						++tmpadder;
					}

					// add sth to b2!!!!!!
					tmpk2.start = newcompressed.tellg();
					newcompressed.write(b2, tmpk2.length);
					newkey.write((char*)&tmpk2, sizeof(KeyNode));
					delete[] b2;
				}
				++j;
				if(j < len2){
					fk2.read((char*)&tmpk2, sizeof(KeyNode));
				} else {
					tmpk2.key = 0xffffffff;//-1; // is it safe? or UINT_MAX?
				}
			} else {
				if((tmpk1.length + tmpk2.length) > 0) {
					b1 = new char[tmpk1.length + tmpk2.length];

					if (tmpk1.length > 0){
						fc1.read(b1, tmpk1.length);
					}
					if (tmpk2.length){
						fc2.read(b1 + tmpk1.length, tmpk2.length);
					}
					// do sth to the 2nd block.
					// same to aforehead. how to remove it, because of the flexibility.
					int count = tmpk2.length / sizeof(int);
					unsigned int *tmpadder = (unsigned int *)(b1 + tmpk1.length);
					while(count-- > 0){
						(*tmpadder) += file_number_offset;
						++tmpadder;
					}
					//need to sort? maybe no.
					tmpk1.start = newcompressed.tellg();
					tmpk1.length += tmpk2.length;
					newcompressed.write(b1, tmpk1.length);
					newkey.write((char*)&tmpk1, sizeof(KeyNode));

					delete[] b1;
				}
				++i; ++j;
				// errr... make sure all i and j are processed.
				if(i < len1){
					fk1.read((char*)&tmpk1, sizeof(KeyNode));
				} else {
					tmpk1.key = 0xffffffff;//-1; // is it safe? or UINT_MAX?
				}
				if(j < len2){
					fk2.read((char*)&tmpk2, sizeof(KeyNode));
				} else {
					tmpk2.key = 0xffffffff;//-1; // is it safe? or UINT_MAX?
				}
			}
		}
		// really? is that ok???
		newkey.close();
		newcompressed.close();
		fk1.close();
		fk2.close();
		fc1.close();
		fc1.close();
	}

} // end namespace bible.

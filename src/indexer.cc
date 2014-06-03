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

using namespace std;

/************************************/

unsigned int getFileNumber(const char* filename) {
	unsigned int num;
	FILE* fout;
	FileNode filename_node = {0};
	ensureFileExists(fcontainer);
	num=getFileLength(fcontainer)/sizeof(FileNode);
	fout=fopen(fcontainer, "rb+");
	fseek(fout, 0, SEEK_END);
	strcpy(filename_node, filename);
	fwrite(filename_node, sizeof(FileNode), 1, fout);
	fclose(fout);
	return num;
}

void addFileToIndex(const char * filename, const char * to) {
	char * raw_string = NULL;
	size_t filelength = 0;

	unsigned int filecount = getFileNumber(filename);
	ensureFileExists(to);

	loadFile(filename, raw_string, filelength);

	auto tokenizer = new TokenizerEnglish();
	auto hashlist = tokenizer->Tokenize(raw_string);

	for (auto item = hashlist->begin(); item != hashlist->end(); ++item) {
		// donno how to convert item to void*. therefore write an ugly line.
		TokenItem* current_item = &(*item);
		current_item->offset = makeFileNode(filecount, current_item->offset);
	}

	if (hashlist->size()){
		fstream file (to, ios::in|ios::out|ios::binary|ios::ate);
		if (file.is_open()) {
		file.write((const char *)(&((*hashlist)[0])),sizeof(IndexNode) * hashlist->size());
		file.close();
	printf("Processing %s, length: %lu bytes. %f\n", filename, filelength, (double)hashlist->size() * sizeof(IndexNode) / filelength);
	}
}
delete hashlist;
}

int sortIndex(const char * filename) {
	int filelength=getFileLength(filename);
	int tmpint=filelength/sizeof(CompareNode);
	FILE* filenameindex=fopen(filename,"rb+");
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

int compressIndex(const char * filename_raw, const char * filename_compress, const char * filename_keyindex) {
	int filelength = getFileLength(filename_raw);
	int tmpint = filelength / sizeof(CompareNode);
	int currentp = 0, m1;
	FILE* filenameindex;
	FILE* compressf;
	FILE* keyf;
	KeyNode tmpkeynode;
	TokenItem *tmp = new TokenItem[tmpint];
	filenameindex = fopen(fraw,"rb");
	fseek(filenameindex,0,SEEK_SET);
	fread(tmp,1,filelength,filenameindex);
	fclose(filenameindex);
	compressf=fopen(filename_compress,"wb");
	rewind(compressf);
	keyf=fopen(filename_keyindex,"wb");
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
	printf("Compress index OK...\n");
	return tmpint;
}
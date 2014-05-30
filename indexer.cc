#ifndef _INDEXER_H__
#define _INDEXER_H__

unsigned int getFileNumber(const char* filename);
void addFileToIndex(char* filename);
int sortIndex();

int compressIndex();
void ShowFile();
#endif


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "tokenizer_english.h"
#include "structure.h"
#include "file_op.h"

using namespace std;

/************************************/

unsigned int getFileNumber(const char* filename) {
	unsigned int num;
	FILE* sss;
	FileNode aaaa;
	ensureFileExists(fcontainer);
	num=getFileLength(fcontainer)/sizeof(FileNode);
	sss=fopen(fcontainer,"rb+");
	fseek(sss,0,SEEK_END);
	strcpy(aaaa,filename);
	fwrite(aaaa,sizeof(FileNode),1,sss);
	fclose(sss);
	return num;
}

void addFileToIndex(const char* filename) {
	char * raw_string = NULL;
	size_t filelength = 0;
	FILE * rawfile;
	unsigned int filecount = getFileNumber(filename);
	ensureFileExists(fraw);

	loadFile(filename, raw_string, filelength);

	rawfile=fopen(fraw,"rb+");
	fseek(rawfile,0, SEEK_END);

	auto tokenizer = new TokenizerEnglish();
	auto hashlist = tokenizer->Tokenize(raw_string);

	for (auto item = hashlist->begin(); item != hashlist->end(); ++item) {
		item->index = makeFileNode(filecount, item->index);
		fwrite(item,sizeof(IndexNode),1,rawfile);///< actually can be written to file altogether.
	}
	printf("Processing %s, length: %d bytes. %f\n",filename,filelength,((float)list.size)/aaa->length);

	fclose(rawfile);
}

int sortIndex() {
	int filelength=getFileLength(fraw);
	int tmpint=filelength/sizeof(CompareNode);
	FILE* filenameindex=fopen(fraw,"rb+");
	auto tmp = new CompareNode[tmpint];
	
	debug_print("Sort index start...\n");
	fseek(filenameindex, 0, SEEK_SET);
	rewind(filenameindex);
	fread(tmp, sizeof(CompareNode), tmpint, filenameindex);
	sort(tmp,tmpint,sizeof(comparenode),compareindex);
	fseek(filenameindex,0,SEEK_SET);
	rewind(filenameindex);
	fwrite(tmp,sizeof(comparenode),tmpint,filenameindex);
	fclose(filenameindex);
	debug_print("Sort index OK...\n");

	free(tmp);
	return tmpint;
}

int main() {
	indexnode a;
	FILE* sss;
	int len;
	char fileitem[256];
	InitSplit();
	printf("Add files to index...\n");

	while(scanf("%s",fileitem)==1)
	{
		addfiletoindex(fileitem);
	}

	SortIndex();

	return 0; 
}
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "file_op.h"
#include "hash.h"
#include "structure.h"
#include "tokenizer.h"
#include "tokenizer_english.h"
#include "search.h"
#include "common.h"

void initSearch() {
	keynodelength = getFileLength(fkeyindex)/sizeof(KeyNode);
}

MemBlock* getMemBlock(CompareNode node) {
	FILE* indexfile;
	if(node.n2 <= 0) {
		return new MemBlock();
	}
	auto res = new MemBlock(node.n2);
	indexfile=fopen(fcompressindex,"rb");
	fseek(indexfile, node.n1, SEEK_SET);
	fread(res->block,1 , node.n2, indexfile);
	fclose(indexfile);
	return res;
}

void makeNext(MemBlock* &m1) {
	unsigned int* tmp = (unsigned int*)(m1->block);
	int length = m1->length/sizeof(int);
	for(int i=0; i<length; ++i){
		//printf("%x -> %x\n", tmp[i], tmp[i]+1);
		++(*(tmp++));
	}
}

int compareBlock(MemBlock* m1, MemBlock* m2) {
	unsigned int* a = (unsigned int*)(m1->block);
	unsigned int* b = (unsigned int*)(m2->block);
	int i = m1->length/sizeof(int), j = m2->length/sizeof(int);
	int count=0;
	unsigned int* tmp;
	tmp = (unsigned int*)m1->block;
	while(i>0 && j>0) {
		//debug_print("Comparing: %x, %x\n", *a, *b);
		if(*a==*b) {
			++count;
			--i; --j;
			*tmp = *a;
			++a;
			++b;
			++tmp;
		} else if (*a < *b) {
			--i;
			++a;
		} else if(*a > *b) {
			--j;
			++b;
		}
	}
	m1->length = count * sizeof(int);
	return count;
}


int showResult(MemBlock* m1) {
	int length = m1->length/sizeof(unsigned int);
	FileNode tmpf;
	FILE* filenameindex=fopen(fcontainer,"rb");
	unsigned int* tmp = (unsigned int*)(m1->block);
	unsigned int filenumber;

	for(int i=0; i<length; i++) {
		filenumber=getfilenumber(tmp[i]);
		fseek(filenameindex,filenumber*sizeof(FileNode),SEEK_SET);
		fread(tmpf,sizeof(FileNode),1,filenameindex);
		printf("%x\t->\t%s\n",filenumber,tmpf);
	}
	fclose(filenameindex);
	return 0;
}

int showCompleteResult(SearchResult* abc)
{
	int i,length=abc->resultcount;
	FileNode tmpf;
	FILE* filenameindex=fopen(fcontainer,"rb");
	unsigned int* tmp;
	unsigned int filenumber;
	tmp=abc->result_index;
	printf("\n--------\nFind  %d results. Time elapse:%g s\n--------\n",length,abc->elapsetime);
	//memset(&tmpf,sizeof(FileNode),0);
	for(i=0;i<length;i++)
	{
		filenumber=getfilenumber(tmp[i]);
		fseek(filenameindex,filenumber*sizeof(FileNode),SEEK_SET);
		fread(tmpf,sizeof(FileNode),1,filenameindex);
		printf("%x\t->\t%s\n",filenumber,tmpf);
	}
	fclose(filenameindex);
	return 0;
}

CompareNode findKey(unsigned int key) {
	int l, m, r; // left, middle, right.
	KeyNode tmpk;
	CompareNode returnvalue;
	FILE* keyindexf=fopen(fkeyindex,"rb");
	returnvalue.n2 = 0;
	returnvalue.n1 = 0;
	
	l = 1;
	r = keynodelength - 1;
	m = (l + r) / 2;
	while(l<=m && m<=r) {
		fseek(keyindexf,m*sizeof(KeyNode),SEEK_SET);
		fread((char*)(&tmpk),sizeof(KeyNode),1,keyindexf);
		if(tmpk.key == key) {
			returnvalue.n1 = tmpk.start;
			returnvalue.n2 = tmpk.length;
			break;
		}
		else if(tmpk.key > key) {
			r = m - 1; m = (l + r)/2;
		}
		else if(tmpk.key < key) {
			l = m + 1; m=(l + r)/2;
		}
	}
	fclose(keyindexf);
	return returnvalue;
}

vector<TokenItem> * parseKeywords(const char* str) {
	auto tokenizer = new TokenizerEnglish();
	auto result = tokenizer->Tokenize(string(str));
	return result;
}

void search(SearchResult* abc, const char* keyword) {
	CompareNode start_and_length;
	MemBlock *m1, *m2;
	StopWatch watch; // create a stopwatch.
	vector<TokenItem> *keywords = parseKeywords(keyword);
	int lsize = keywords->size();

	if (lsize > 0) {

		start_and_length = findKey(keywords->at(0).hash);

		if(start_and_length.n2) {
			m1 = getMemBlock(start_and_length);
			for(int i=1; i<lsize; ++i) {
			//debug_print("dealing with %d, size:%d...\n", keywords->at(i).hash, start_and_length.n2);
				start_and_length = findKey(keywords->at(i).hash);
				if (start_and_length.n2) {
					m2 = getMemBlock(start_and_length);
					makeNext(m1);
					size_t count = compareBlock(m1, m2);
					if (!count) {				
						m1->Free();
						m2->Free();
						break;
					}
					delete m2;
				}
			}
			abc->result_index = (unsigned int *)m1->block;
			abc->result = NULL;
			abc->resultcount = m1->length/sizeof(int);
		}
	} else {
		abc->resultcount = 0;
		abc->result_index = NULL;
		abc->result = NULL;
	}
	abc->elapsetime = watch.Stop();
	delete keywords;
}

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "structure.h"

const char* fkeyindex="keyindex.index";
const char* fcontainer="container.index";
const char* fraw="raw.index";
const char* fcompressindex="compress.index";
int keynodelength;

/**
 * compareindex: compare two node.
 * @param[in] a 
 * @param[in] b 
 * @return -1: less than; 0: eq; 1 greater than
*/
int compareIndex(const CompareNode a, const CompareNode b){
	if (a.n1==b.n1) {
		return a.n2 < b.n2;
	}
	else{
		return a.n1 < b.n1;
	}
}

MemBlock::MemBlock(size_t size){
	if (size == 0){
		this->block = NULL;
	} else {
		this->block = new char[size];
	}
	this->length = size;
}

MemBlock::MemBlock(){
	MemBlock(0);
}

MemBlock::~MemBlock(){
	this->Free();
}

void MemBlock::Free() {
	this->length = 0;
	if (this->block!=NULL) {
		delete[] this->block;
		this->block = NULL;
	}

}


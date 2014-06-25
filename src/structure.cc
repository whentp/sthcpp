#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
namespace bible{
	/**
	 * compareindex: compare two node.
	 * @param[in] a 
	 * @param[in] b 
	 * @return -1: less than; 0: eq; 1 greater than
	 */
	bool compareIndex(const CompareNode a, const CompareNode b){
		if (a.n1==b.n1) {
			return a.n2 < b.n2;
		}
		else{
			return a.n1 < b.n1;
		}
	}

	MemBlock::MemBlock(size_t size): flag(0) {
		if (size == 0){
			this->block = NULL;
		} else {
			this->block = new char[size];
		}
		this->length = size;
	}

	MemBlock::~MemBlock(){
		this->Free();
	}

	void MemBlock::Lock(){
		flag = 1;
	}

	void MemBlock::Free() {
		this->length = 0;
		if (flag) return; // do not delete block when flag = 1;
		if (this->block!=NULL) {
			delete[] this->block;
			this->block = NULL;
		}
	}

	void SearchResult::SetMsg(const char* str){
		size_t len = strlen(str) + 1;
		msg = new char[len];
		strcpy(msg, str);
		msg[len] = 0;
	}

	SearchResult::~SearchResult(){
		if (indexes) delete[] indexes;
		if (filenames){
			for(size_t i = 0; i < count; ++i){
				delete filenames[i];
			}
			delete[] filenames;
		}
		if (msg){
			delete msg;
		}
	}

} // end namespace bible.

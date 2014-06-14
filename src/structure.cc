#include <stdio.h>
#include <stdlib.h>
#include "structure.h"
namespace bible{
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
		if(flag) return; // do not delete block when flag = 1;
		this->length = 0;
		if (this->block!=NULL) {
			delete[] this->block;
			this->block = NULL;
		}
	}

	SearchResult::~SearchResult(){
		if (result_index) delete[] result_index;
		if(filenames){
			for(size_t i = 0; i < resultcount; ++i){
				delete filenames[i];
			}
			delete[] filenames;
		}
	}

} // end namespace bible.

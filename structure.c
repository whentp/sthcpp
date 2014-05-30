#include "structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

void FreeMemBlock(struct memblock* aaa){
	aaa->length=0;
	if (aaa->block!=NULL)
	{
		free(aaa->block);
	}

}

/**
 * compareindex: compare two node.
 * @param[in] a 
 * @param[in] b 
 * @return -1: less than; 0: eq; 1 greater than
*/
int compareIndex(const void* a,const void* b){
	comparenode c=*(comparenode*)a;
	comparenode d=*(comparenode*)b;
	if (c.n1==d.n1) {
		return c.n2 - d.n2;
	}
	else{
		return c.n1 - d.n1;
	}
}
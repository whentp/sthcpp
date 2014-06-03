#include "structure.h"
#include "tokenizer.h"

#ifndef _SEARCHER_H__
#define _SEARCHER_H__

void initSearch();
MemBlock* getMemBlock(CompareNode node);
void makeNext(MemBlock* &m1);
int compareBlock(MemBlock* m1, MemBlock* m2);
int showResult(MemBlock* m1);
int showCompleteResult(SearchResult* abc);
CompareNode findKey(unsigned int key);
vector<TokenItem> * parseKeywords(const char* str); 
void search(SearchResult* abc, const char* keyword);

#endif

#ifndef _SEARCHER_H__
#define _SEARCHER_H__

#include "structure.h"
#include "tokenizer.h"
#include "keyword_tree.h"

void initSearch();
void makeNext(MemBlock* &m1);
int compareBlock(MemBlock* m1, MemBlock* m2);
//int showResult(MemBlock* m1);
//int showCompleteResult(SearchResult* abc);
vector<TokenItem> * parseKeywords(const char* str); 

MemBlock* getMemBlock(
	CompareNode node, 
	const char* fcompressindex);

CompareNode findKey(
	unsigned int key,
	const char *fkeyindex);

void searchSingleKeyword(
	SearchResult* abc, 
	const char* keyword,
	const char* fkeyindex,
	const char* fcompressed);

SearchResult *searchMultipleKeywords( 
	const char* keyword,
	const char* fkeyindex,
	const char* fcompressed);

SearchResult *searchByKeywordTree(
	KeywordTree *kt,
	const char* fkeyindex,
	const char* fcompressed);

void matchFilenamesForResults(
	SearchResult* res,
	const char* fcontainer);

void shrinkSearchSingleKeyword(SearchResult* res);

size_t mergeSearchSingleAnd(SearchResult* a, SearchResult* b);
size_t mergeSearchSingleOr(SearchResult* a, SearchResult* b);

#endif
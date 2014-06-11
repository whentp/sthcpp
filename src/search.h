
#ifndef _SEARCHER_H__
#define _SEARCHER_H__

#include <fstream>
#include "structure.h"
#include "tokenizer.h"
#include "keyword_tree.h"
namespace bible{

	class KeyIndex{
		private:
			ifstream keyindexf;
			size_t keynodelength;
		public:
			KeyIndex(const char *fkeyindex);
			~KeyIndex();
			CompareNode Find(unsigned int key);
	};

	class Searcher{
		private:
			const char* _fcontainer;
			const char* _fkeyindex;
			const char* _fcompressed;
			KeyIndex *_keyindex;
		public:
			Searcher(
					const char* fcontainer,
					const char* fkeyindex,
					const char* fcompressed);
			~Searcher();
			SearchResult *Search(const char *keyword_str);
			void MatchFilenames(SearchResult* res);
	};

	void makeNext(MemBlock* &m1);
	int compareBlock(MemBlock* m1, MemBlock* m2);

	vector<TokenItem> * parseKeywords(const char* str); 

	MemBlock* getMemBlock(
			CompareNode node, 
			const char* fcompressindex);

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

	size_t mergeSearchSingleAnd(SearchResult* a, SearchResult* b); ///< a & b
	size_t mergeSearchSingleOr(SearchResult* a, SearchResult* b); ///< a | b
	size_t mergeSearchSingleSub(SearchResult* a, SearchResult* b); ///< a - b
} // end namespace bible.
#endif

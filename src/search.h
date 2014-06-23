
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
		CompareNode Find(BibleIntType key);
	};

	class Searcher{
	private:
		string _fcontainer;
		string _fkeyindex;
		string _fcompressed;
		KeyIndex *_keyindex_finder;

		ifstream _indexfile;

		void SearchSingleKeyword(
			SearchResult* res, 
			const char* keyword);
		SearchResult *SearchMultipleKeywords(const char* keyword);
		SearchResult *SearchByKeywordTree(const KeywordTree *kt);
		
		MemBlock *GetMemBlock(CompareNode &node);
	public:
		Searcher(
			const char* fcontainerstr,
			const char* fkeyindexstr,
			const char* fcompressedstr);
		~Searcher();
		SearchResult *Search(const char *keyword_str);
		void MatchFilenames(SearchResult* res);
	};

	void makeNext(MemBlock* &m1);
	size_t compareBlock(MemBlock* m1, MemBlock* m2);

	vector<TokenItem> * parseKeywords(const char* str); 

	void matchFilenamesForResults(
		SearchResult* res,
		const char* fcontainer);

	void shrinkSearchSingleKeyword(SearchResult* res);

	size_t mergeSearchSingleAnd(SearchResult* a, SearchResult* b); ///< a & b
	size_t mergeSearchSingleOr(SearchResult* a, SearchResult* b); ///< a | b
	size_t mergeSearchSingleSub(SearchResult* a, SearchResult* b); ///< a - b
} // end namespace bible.
#endif

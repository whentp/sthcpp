#ifndef _KEYWORD_TREE_H__
#define _KEYWORD_TREE_H__

#include <string>
namespace bible{
	using namespace std;

	enum KeywordType{
		KT_STRING, ///< common string, i.e., [\w\d]+
		KT_AND,  ///< &
		KT_OR,	///< |
		KT_SUB,	///< -
		KT_NOT	///< ^, not implemented.
	};

	struct KeywordTree{
		KeywordType type;
		KeywordTree *left;
		KeywordTree *right;
		string keyword;
		KeywordTree(): KeywordTree(KT_STRING){}
		KeywordTree(KeywordType t);
		~KeywordTree();
	};

	KeywordTree *parseKeywordTree(string str);
} // end namespace bible.
#endif

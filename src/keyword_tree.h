#ifndef _KEYWORD_TREE_H__
#define _KEYWORD_TREE_H__

#include <string>

using namespace std;

enum KeywordType{ KT_STRING, KT_AND, KT_OR, KT_NOT};

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

#endif
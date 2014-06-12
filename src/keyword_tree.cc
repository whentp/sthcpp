#include <string>
#include "keyword_tree.h"
#include <iostream>
namespace bible{
	using namespace std;

	KeywordTree::KeywordTree(KeywordType t){
		left = NULL;
		right = NULL;
		type = t;
	}

	KeywordTree::~KeywordTree(){
		if (left) delete left;
		if (right) delete right;
	}

	KeywordTree *parseKeywordTree(string str){
		KeywordTree *res = new KeywordTree();
		// KT_SUB (-) has the lowest priority.
		// KT_AND (&) has the highest priority.
		//cout << "left\t" << str.substr(0, str.find("|")) <<endl;
		//cout << "right\t" << str.substr(str.find("|")+1) <<endl;
		if(str.find("-")!=string::npos){
			res->type = KT_SUB;
			res->left = parseKeywordTree(str.substr(0, str.find("-")));
			res->right = parseKeywordTree(str.substr(str.find("-")+1));
		} else if(str.find("|")!=string::npos){
			res->type = KT_OR;
			res->left = parseKeywordTree(str.substr(0, str.find("|")));
			res->right = parseKeywordTree(str.substr(str.find("|")+1));
		} else if(str.find("&")!=string::npos){
			res->type = KT_AND;
			res->left = parseKeywordTree(str.substr(0, str.find("&")));
			res->right = parseKeywordTree(str.substr(str.find("&")+1));
		} else {
			res->type = KT_STRING;
			res->keyword = str;
			res->left = NULL;
			res->right = NULL;
		}
		return res;
	}
} // end namespace bible.

#include <string>
#include "keyword_tree.h"
#include <iostream>
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
	//int len = str.size();
	if(str.find("&")!=string::npos){
		res->type = KT_AND;
		//cout<< str.substr(0, str.find("&")) <<endl;
		//cout<< str.substr(str.find("&")+1) <<endl;
		res->left = parseKeywordTree(str.substr(0, str.find("&")));
		res->right = parseKeywordTree(str.substr(str.find("&")+1));
	} else if(str.find("|")!=string::npos){
		res->type = KT_OR;
		//cout<< str.substr(0, str.find("|")) <<endl;
		//cout<< str.substr(str.find("|")+1) <<endl;
		res->left = parseKeywordTree(str.substr(0, str.find("|")));
		res->right = parseKeywordTree(str.substr(str.find("|")+1));
	} else {
		res->type = KT_STRING;
		res->keyword = str;
		res->left = NULL;
		res->right = NULL;
	}
	return res;
}
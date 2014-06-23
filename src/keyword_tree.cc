#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "keyword_tree.h"

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

	KeywordTree *parseKeywordTree(const string str){
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


	vector<string> *queryTokenizer(const string &str){
		vector<string> *res = new vector<string>();
		map<char, char> excape_chars = {
			{'t','\t'},{'n','\n'},{'r','\r'},{'"','"'},{'\'','\''},
			{'(','('},{'\\','\\'},{')',')'},
			{'&','&'},{'|','|'},{'-','-'},{'^','^'}};
		map<char, bool> operators = {
			{'&',true},{'|',true},{'-',true},
			{'^',true},{'(',true},{')',true}};
		map<char, bool> white_chars = {
			{' ',true},{'\t',true},{'\n',true},{'\r',true}};

		//cout << "start tokenizing: " << str << endl;
		size_t current = 0, previous = 0;
		bool is_quoted = false;
		//bool is_escape = false;
		while(current < str.size()){
			previous = current;
			while (current < str.size() && (is_quoted || operators.find(str.at(current)) == operators.end())) {
				// skip leading white chars.
				while((previous == current) && white_chars.find(str.at(current)) != white_chars.end()){
					++current;
					previous = current;
					//cout << "hi" << endl;
				}
				//cout << "non white start: " << current << "," << str.at(current) << endl;
				if('\\' == str.at(current)){
					//just skip it.
					++current;
					++current;
				} else if('"' == str.at(current)){
					//cout << "quote." << endl;
					if(is_quoted){
						break;
					} else {
						if(previous == current){
							is_quoted = true;
							++previous;
						}
						++current;
					}
				} else {
					++current;
				}
			}
			//cout << current << "," << previous << "," << str.size() << "," << str.substr(previous, current - previous) << endl;
			if(is_quoted){
				is_quoted = false;
				if(current < str.size()){
					res->push_back(str.substr(previous, current - previous));
					++current;
				} else {
					if(current >= str.size()){
						res->push_back(str.substr(previous));	
					} else {
						res->push_back(str.substr(previous, current - previous));
					}
				}
			} else {
				if(current < str.size()){
					if(current != previous){
						res->push_back(str.substr(previous, current - previous));
					}
					if(str.at(current) == '"'){
						continue;
					}
					res->push_back(str.substr(current, 1));
					//cout << ".." << endl;
				} else {
					res->push_back(str.substr(previous, current - previous));
				}
				++current;
			}
		}
		return res;
	}

	KeywordTree *parseKeywordTreeStrict(const string &str){
		return new KeywordTree();
	}


} // end namespace bible.

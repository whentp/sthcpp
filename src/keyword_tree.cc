/**
 * (C) Copyright 2014.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 3.0 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     whentp <tpsmsproject@gmail.com>
 */

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

	void escapeString(string &str, const map<char, char> &dict){
		size_t previous = 0, current = 0;
		while(current < str.size()){
			if(str.at(current) == '\\'){
				++current;
				if(current < str.size()){
					auto tmpfind = dict.find(str.at(current));
					if(tmpfind != dict.end()){
						str.at(previous) = tmpfind->second;
						//++current;
						//++previous;
					} else {
						throw "escape string error.";
					}
				}
			} else {
				str.at(previous) = str.at(current);
			}
			++current;
			++previous;
		}
		str.resize(previous);
	}

	vector<string> *queryTokenizer(const string &str){
		vector<string> *res = new vector<string>();
		map<char, char> escape_chars = {
			{'t','\t'},{'n','\n'},{'r','\r'},{'"','"'},{'\'','\''},
			{'(','('},{'\\','\\'},{')',')'},
			{'&','&'},{'|','|'},{'-','-'},{'^','^'}
		};
		map<char, bool> operators = {
			{'&',true},{'|',true},{'-',true},
			{'^',true},{'(',true},{')',true}
		};
		map<char, bool> white_chars = {
			{' ',true},{'\t',true},{'\n',true},{'\r',true}
		};

		size_t current = 0, previous = 0;
		bool is_quoted = false;
		while(current < str.size()){
			previous = current;
			while (current < str.size() && (is_quoted || operators.find(str.at(current)) == operators.end())) {
				// skip leading white chars.
				while((previous == current) && white_chars.find(str.at(current)) != white_chars.end()){
					++current;
					previous = current;
				}
				if('\\' == str.at(current)){
					++current;
					if(current < str.size()){
						++current;
					}
				} else if('"' == str.at(current)){
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
						continue; // is it correct?!?!?!?!? yes. seriously.
					}
					res->push_back(str.substr(current, 1));
				} else {
					res->push_back(str.substr(previous, current - previous));
				}
				++current;
			}
		}
		return res;
	}

	void buildKeywordTree(const vector<string> *tokens){
		int check_brackets = 0;
		for(auto &str : *tokens){
			if(str == "("){
				++check_brackets;
			} else if(str == ")"){
				--check_brackets;
			}
			if(check_brackets < 0){
				throw "unmatched bracket(s).";
			}
		}
		if(check_brackets != 0){
			throw "unmatched bracket(s).";
		}	
	}

	KeywordTree *parseKeywordTreeStrict(const string &str){
		return new KeywordTree();
	}

} // end namespace bible.

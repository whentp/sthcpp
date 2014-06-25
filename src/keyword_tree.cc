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
#include "exceptions.h"

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

	KeywordTree *parseKeywordTreeRough(const string str){
		KeywordTree *res = new KeywordTree();
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
					} else {
						BIBLE_THROW(ExceptionBible, "escape string error.");
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

	void checkBracktes(const vector<string> *tokens){
		int check_brackets = 0;
		for(auto &str : *tokens){
			if(str == "("){
				++check_brackets;
			} else if(str == ")"){
				--check_brackets;
			}
			if(check_brackets < 0){
				BIBLE_THROW(ExceptionBible, "unmatched bracket(s).");
			}
		}
		if(check_brackets != 0){
			BIBLE_THROW(ExceptionBible, "unmatched bracket(s).");
		}

	}

	KeywordTree *getKeywordNode(
			const vector<string> *tokens,
			const size_t start,
			size_t &end)
	{
		if(tokens->size() <= start){
			BIBLE_THROW(ExceptionBible, "build tree error.");
		}
		string current_token = tokens->at(start);
		if("&" == current_token){
			KeywordTree *res = new KeywordTree(KT_AND);
			end = start + 1;
			return res;
		} else if("|" == current_token){
			KeywordTree *res = new KeywordTree(KT_OR);
			end = start + 1;
			return res;
		} else if("-" == current_token){
			KeywordTree *res = new KeywordTree(KT_SUB);
			end = start + 1;
			return res;
		}  else if("(" != current_token){
			KeywordTree *res = new KeywordTree(KT_STRING);
			res->keyword = current_token;
			end = start + 1;
			return res;
		} else { //if("(" == current_token)
			//deal with brackets.
			int i = 0;
			size_t paired_bracket = 0;
			//cout << "start: "<< start <<", end: "<< tokens->size() << "current: " << tokens->at(start)<<endl;
			for(paired_bracket = start; paired_bracket < tokens->size(); ++paired_bracket){
				if(tokens->at(paired_bracket) == "("){
					++i;
				} else if(tokens->at(paired_bracket) == ")"){
					--i;
				}
				if(i == 0){
					break;
				}
			}
			//cout << "paired close bracket:" << paired_bracket << endl;
			if(1 == (paired_bracket - start)){
				BIBLE_THROW(ExceptionBible, "nothing in brackets.");
			}
			KeywordTree *res = buildKeywordTree(tokens, start + 1, paired_bracket);
			end = paired_bracket + 1;
			return res;
		}
	}

	KeywordTree *buildKeywordTree(
			const vector<string> *tokens,
			const size_t start,
			const size_t end)
	{
		size_t stop = end;
		size_t current = start;

		KeywordTree *node1 = getKeywordNode(tokens, current, current);
		// res is the return value.
		KeywordTree *res = node1;

		while(current < stop){
			KeywordTree *node2 = getKeywordNode(tokens, current, current);
			if (KT_AND == node2->type){
				res = node2;
				res->left = node1;
				res->right = getKeywordNode(tokens, current, current);
			} else if(KT_OR == node2->type){
				res = node2;
				res->left = node1;
				res->right = getKeywordNode(tokens, current, current);
			} else if(KT_SUB == node2->type){
				res = node2;
				res->left = node1;
				res->right = getKeywordNode(tokens, current, current);
			} else if(KT_STRING == node2->type){
				res = new KeywordTree(KT_AND);
				res->left = node1;
				res->right = node2;
			} else {

			}
			node1 = res;
		}
		return res;
	}

	KeywordTree *parseKeywordTreeStrict(const string &str){
		string query_string = str;
		vector<string> *tokens = queryTokenizer(query_string);
		checkBracktes(tokens);
		KeywordTree *res = buildKeywordTree(tokens, 0, tokens->size());
		return res;
	}

	void printKeywordTree(KeywordTree *tmp, size_t level){
		if(tmp->type == KT_STRING){
			cout << tmp->keyword << endl;
		} else {
			cout << "type: " << tmp->type << endl;
			if(tmp->left){
				for(size_t i = 0; i < level; ++i){
					cout << "     ";
				}
				cout << "|----";
				printKeywordTree(tmp->left, level+1);
			}

			if(tmp->right){
				for(size_t i = 0; i < level; ++i){
					cout << "     ";
				}
				cout << "|----";
				printKeywordTree(tmp->right, level+1);
			}
		}
	}

	KeywordTree *parseKeywordTree(const string str, bool rough){
		if(rough){
			return parseKeywordTreeRough(str);
		} else {
			return parseKeywordTreeStrict(str);
		}
	}

	KeywordTree *parseKeywordTree(const string str){
		return parseKeywordTree(str, false);
	}

} // end namespace bible.

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
 
#ifndef _KEYWORD_TREE_H__
#define _KEYWORD_TREE_H__

#include <string>
#include <vector>
#include <map>

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

void escapeString(string &str, const map<char, char> &dict);

	KeywordTree *parseKeywordTree(const string str);

vector<string> *queryTokenizer(const string &str);

KeywordTree *parseKeywordTreeStrict(const string &str);


} // end namespace bible.
#endif

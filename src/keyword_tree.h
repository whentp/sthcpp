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
		KT_NOT,	///< ^, not implemented.
		KT_SINGLE ///< if there is only one child. e.g., ((x)). 
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

	/**
	 * @brief check if all brackets are closed properly.
	 * 
	 * @param tokens the token list.
	 */
	void checkBracktes(const vector<string> *tokens);

	/**
	 * @brief get a keynodetree node. and set the parameter end to where the next where node starts.
	 * 
	 * @param[in] tokens the token list.
	 * @param[in] start the index the node start at.
	 * @param[out] end set the parameter end to the index the next where node starts at.
	 * @return the keyword node.
	 */
	KeywordTree *getKeywordNode(
			const vector<string> *tokens,
			const size_t start,
			size_t &end);

	/**
	 * @brief construct keyword tree.
	 * 
	 * @param[in] tokens the token list.
	 * @param[in] start the index the node start at.
	 * @param[in] end the length of the token list.
	 * @return the keyword node.
	 */
	KeywordTree *buildKeywordTree(
			const vector<string> *tokens,
			const size_t start,
			const size_t end);

	/**
	 * @brief Print keyword tree.
	 * 
	 * @param tmp the keyword tree to be printed.
	 * @param level indent number of the root node.
	 */
	void printKeywordTree(KeywordTree *tmp, size_t level);

	/**
	 * @brief tokenize query string.
	 */
	vector<string> *queryTokenizer(const string &str);

	/**
	 * @brief parse query string according to their grammar.
	 */
	KeywordTree *parseKeywordTreeStrict(const string &str);

	/**
	 * @brief parse query string by just spliting it. quite easy.
	 */
	KeywordTree *parseKeywordTreeRough(const string str);

	KeywordTree *parseKeywordTree(const string str, bool rough);
	KeywordTree *parseKeywordTree(const string str);


} // end namespace bible.
#endif

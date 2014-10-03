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

#ifndef _SEARCHER_H__
#define _SEARCHER_H__

#include <fstream>
#include "keyword_tree.h"
#include "keyindex.h"
#include "tokenizer.h"
#include "structure.h"

namespace bible {

class Searcher {
private:
    string _fcontainer;
    string _fkeyindex;
    string _fcompressed;
    string _tokenizer_name;
    KeyIndex *_keyindex_finder;
    ifstream _indexfile;

    void SearchSingleKeyword(
        SearchResult *res,
        const char *keyword);
    MemBlock *GetMemBlock(CompareNode &node);
public:
    Searcher(
        const char *fcontainerstr,
        const char *fkeyindexstr,
        const char *fcompressedstr,
        const char *tokenizer_name);

    ~Searcher();

    SearchResult *SearchByKeywordTree(const KeywordTree *kt);

    SearchResult *SearchMultipleKeywords(const char *keyword);

    ///< alias of SearchMultipleKeywords
    SearchResult *Search(const char *keyword_str);

    void MatchFilenames(SearchResult *res);
};

void makeNext(MemBlock *&m1);
size_t compareBlock(MemBlock *m1, MemBlock *m2);

vector<TokenItem> *parseKeywords(const char *str, const string &tokenizer_name);

void matchFilenamesForResults(
    SearchResult *res,
    const char *fcontainer);

void shrinkSearchSingleKeyword(SearchResult *res);

size_t mergeSearchSingleAnd(SearchResult *a, SearchResult *b); ///< a & b
size_t mergeSearchSingleOr(SearchResult *a, SearchResult *b); ///< a | b
size_t mergeSearchSingleSub(SearchResult *a, SearchResult *b); ///< a - b
} // end namespace bible.
#endif

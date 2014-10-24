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
#include "search.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include "file_op.h"
#include "hash.h"
#include "common.h"
#include "tokenizer_init.h"

namespace bible {

Searcher::Searcher(const char *fcontainerstr,
                   const char *fkeyindexstr,
                   const char *fcompressedstr,
                   const char *tokenizer_name) {
    _fcontainer = fcontainerstr;
    _fkeyindex = fkeyindexstr;
    _fcompressed = fcompressedstr;
    _tokenizer_name = tokenizer_name;

    _keyindex_finder = new KeyIndex(_fkeyindex.c_str());

    _indexfile.open(_fcompressed.c_str(), ios::in | ios::binary);
    if (!_indexfile.is_open()) {
        cout << "Unable to open file: " << _fcompressed;
        exit(0);
    }

    _filenamefounder = new Container();
    _filenamefounder->Open(_fcontainer.c_str());
}

Searcher::~Searcher() {
    //cout << "searcher destroyed." << endl;
    _indexfile.close();
    delete _keyindex_finder;
    _filenamefounder->Close();
    delete _filenamefounder;
}

SearchResult *Searcher::Search(const char *keyword_str) {
    return SearchMultipleKeywords(keyword_str);
}

void Searcher::MatchFilenames(SearchResult *res) {
    //matchFilenamesForResults(res, _fcontainer.c_str());
    size_t length = res->count;
    BibleIntType *tmp;
    BibleIntType filenumber;
    tmp = res->indexes;

    res->filenames = new FileNode[length];
    for (size_t i = 0; i < length; ++i) {
        filenumber = getfilenumber(tmp[i]);
        res->filenames[i] = (FileNode)_filenamefounder->GetFilename(filenumber);
    }
}

MemBlock *Searcher::GetMemBlock(CompareNode &node) {
    if (node.n2 <= 0) {
        return new MemBlock();
    }

    MemBlock *res = new MemBlock(node.n2);
    _indexfile.seekg(node.n1, ios::beg);
    _indexfile.read(res->block, node.n2);
    return res;
}

void makeNext(MemBlock *&m1) {
    BibleIntType *tmp = (BibleIntType *)(m1->block);
    size_t length = m1->length / sizeof(BibleIntType);
    while (length > 0) {
        ++(*(tmp++));
        --length;
    }
}

size_t compareBlock(MemBlock *m1, MemBlock *m2) {
    BibleIntType *a = (BibleIntType *)(m1->block);
    BibleIntType *b = (BibleIntType *)(m2->block);
    size_t i = m1->length / sizeof(BibleIntType), j = m2->length / sizeof(BibleIntType);
    size_t count = 0;
    BibleIntType *tmp;
    tmp = (BibleIntType *)m1->block;
    while (i > 0 && j > 0) {
        //debug_print("Comparing: %x, %x\n", *a, *b);
        if (*a < *b) {
            --i;
            ++a;
        } else if (*a > *b) {
            --j;
            ++b;
        } else { // deal with equal.
            ++count;
            --i; --j;
            *tmp = *a;
            ++a;
            ++b;
            ++tmp;
        }
    }
    m1->length = count * sizeof(BibleIntType);
    return count;
}

void matchFilenamesForResults(SearchResult *res, const char *fcontainer) {
    size_t length = res->count;
    BibleIntType *tmp;
    BibleIntType filenumber;
    tmp = res->indexes;

    Container filenamefounder;
    filenamefounder.Open(fcontainer);

    res->filenames = new FileNode[length];
    for (size_t i = 0; i < length; ++i) {
        filenumber = getfilenumber(tmp[i]);
        res->filenames[i] = (FileNode)filenamefounder.GetFilename(filenumber);
    }
    filenamefounder.Close();
}

vector<TokenItem> *parseKeywords(const char *str, const string &tokenizer_name) {
    auto tokenizer = globalTokenizers[tokenizer_name];
    auto result = tokenizer(string(str));
    return result;
}

SearchResult *Searcher::SearchMultipleKeywords(const char *keyword) {
    string keywordstring(keyword);
    //cout<<"parse tree begin."<<endl;
    KeywordTree *kt = parseKeywordTree(keywordstring);
    //cout<<"parse tree end."<<endl;
    SearchResult *res = SearchByKeywordTree(kt);
    delete kt;
    return res;
}

SearchResult *Searcher::SearchByKeywordTree(const KeywordTree *kt) {
    //cout << "Keytree type:" << kt->type << endl;
    if (KT_STRING == kt->type) {
        SearchResult *res = new SearchResult();
        SearchSingleKeyword(res, kt->keyword.c_str());
        shrinkSearchSingleKeyword(res);
        return res;
    } else if (KT_AND == kt->type) {
        SearchResult *res1 = SearchByKeywordTree(kt->left);
        if (res1->count) {
            SearchResult *res2 = SearchByKeywordTree(kt->right);
            if (res2->count) {
                mergeSearchSingleAnd(res1, res2);
            }
            delete res2;
        }
        return res1;
    } else if (KT_OR == kt->type) {
        SearchResult *res1 = SearchByKeywordTree(kt->left);
        SearchResult *res2 = SearchByKeywordTree(kt->right);
        if (res1->count > 0 && res2->count == 0) {
            delete res2;
            return res1;
        } else if (res1->count == 0 && res2->count > 0) {
            delete res1;
            return res2;
        } else {
            mergeSearchSingleOr(res1, res2);
            delete res2;
            return res1;
        }
    } else if (KT_SUB == kt->type) {
        SearchResult *res1 = SearchByKeywordTree(kt->left);
        SearchResult *res2 = SearchByKeywordTree(kt->right);
        if (res1->count == 0 || res2->count == 0) {
            delete res2;
            return res1;
        } else {
            mergeSearchSingleSub(res1, res2);
            delete res2;
            return res1;
        }
    } else {
        return new SearchResult();
    }
}

void Searcher::SearchSingleKeyword(
    SearchResult *res,
    const char *keyword)
{
    CompareNode start_and_length;
    MemBlock *m1, *m2;
    vector<TokenItem> *keywords = parseKeywords(keyword, _tokenizer_name);
    size_t lsize = keywords->size();

    if (lsize > 0) {
        start_and_length = _keyindex_finder->Find(keywords->at(0).hash);
        //cout << keywords->at(0).hash << endl;
        //cout << "getmemblock: " << start_and_length.n1 << ", len:" << start_and_length.n2 << endl;//<< m1->length << endl;
        if (start_and_length.n2) {
            m1 = GetMemBlock(start_and_length);
            for (size_t i = 1; i < lsize; ++i) {
                //debug_print("dealing with %d, size:%d...\n", keywords->at(i).hash, start_and_length.n2);
                start_and_length = _keyindex_finder->Find(keywords->at(i).hash);
                if (start_and_length.n2) {
                    m2 = GetMemBlock(start_and_length);
                    makeNext(m1);
                    size_t count = compareBlock(m1, m2);
                    if (!count) {
                        m1->Free();
                        m2->Free();
                        break;
                    }
                    delete m2;
                } else {
                    m1->Free();
                    break;
                }
            }
            res->indexes = (BibleIntType *)m1->block;
            m1->Lock();
            res->count = m1->length / sizeof(BibleIntType);
            delete m1;
        } else {
            res->count = 0;
            res->indexes = NULL;
        }
    } else {
        res->count = 0;
        res->indexes = NULL;
    }
    delete keywords;
    // before leaving, m1 & m1->block should be released properly. or leak here.
}

void shrinkSearchSingleKeyword(SearchResult *res) {
    size_t len = res->count;
    if (len == 0) return;
    BibleIntType *cmp = res->indexes, *cur = res->indexes;
    *cur = makeFileNode(getfilenumber(*cur), 0);
    while (len-- > 0) {
        if (getfilenumber(*cmp) == getfilenumber(*cur)) {
            ++(*cur); // yes this equals to 
            //makeFileNode(getfilenumber(*cur), (getfileoffset(*cur) + 1) % getfileoffset(MAX_BIBLE_INT_VALUE)) ;
        } else {
            ++cur;
            *cur = makeFileNode(getfilenumber(*cmp), 1);
        }
        ++cmp;
    }
    res->count = cur - res->indexes + 1;
}

size_t mergeSearchSingleAnd(SearchResult *a, SearchResult *b) {
    size_t i = a->count;
    size_t j = b->count;
    if (i == 0) {
        return 0;
    }
    if (j == 0) {
        delete[] a->indexes;
        a->indexes = NULL;
        a->count = 0;
        return 0;
    }
    BibleIntType *ap = a->indexes;
    BibleIntType *tmp = a->indexes;
    BibleIntType *bp = b->indexes;
    BibleIntType tmpa, tmpb;
    size_t count = 0;
    while (i > 0 && j > 0) {
        tmpa = getfilenumber(*ap);
        tmpb = getfilenumber(*bp);
        if (tmpa < tmpb) {
            --i;
            ++ap;
        } else if (tmpa > tmpb) {
            --j;
            ++bp;
        } else { // deal with equal.
            ++count;
            *tmp = makeFileNode(tmpa, 1);
            ++tmp;
            ++ap;
            ++bp;
            --i; --j;
        }
    }
    a->count = count;
    return count;
}

size_t mergeSearchSingleOr(SearchResult *a, SearchResult *b) {
    size_t len1 = a->count;
    size_t len2 = b->count;
    size_t i = 0, j = 0;
    BibleIntType *ap = a->indexes;
    BibleIntType *bp = b->indexes;
    BibleIntType *tmp = new BibleIntType[len1 + len2];
    BibleIntType tmpa, tmpb;
    size_t count = 0;
    BibleIntType *newresult = tmp;

    i = len1;
    j = len2;

    while (i > 0 && j > 0) {
        tmpa = getfilenumber(*ap);
        tmpb = getfilenumber(*bp);
        if (tmpa < tmpb) {
            *tmp = makeFileNode(tmpa, 1);
            ++tmp;
            --i;
            ++ap;
        } else if (tmpa > tmpb) {
            *tmp = makeFileNode(tmpb, 1);
            ++tmp;
            --j;
            ++bp;
        } else { // deal with equal.
            *tmp = makeFileNode(tmpa, 1);
            ++tmp;
            ++ap;
            ++bp;
            --i; --j;
        }
        ++count;
    }

    // deal with remaining items.
    while (i > 0) {
        *tmp = *ap;
        ++tmp;
        --i;
        ++ap;
        ++count;
    }
    while (j > 0) {
        *tmp = *bp;
        ++tmp;
        --j;
        ++bp;
        ++count;
    }

    a->count = count;

    delete[] a->indexes;
    a->indexes = newresult;
    //cout<<"end or merge."<<count<<endl;
    return count;
}

size_t mergeSearchSingleSub(SearchResult *a, SearchResult *b) {
    size_t i = a->count;
    size_t j = b->count;
    if (j == 0) {
        return i;
    }
    BibleIntType *ap = a->indexes;
    BibleIntType *bp = b->indexes;
    BibleIntType *tmp = ap;
    BibleIntType tmpa, tmpb;
    size_t count = 0;

    while (i > 0 && j > 0) {
        tmpa = getfilenumber(*ap);
        tmpb = getfilenumber(*bp);
        if (tmpa < tmpb) {
            *tmp = makeFileNode(tmpa, 1);
            ++tmp;
            ++count;
            --i;
            ++ap;
        } else if (tmpa > tmpb) {
            --j;
            ++bp;
        } else { // deal with equal.
            ++ap;
            ++bp;
            --i; --j;
        }
    }
    // deal with remaining items.
    while (i > 0) {
        *tmp = *ap;
        ++tmp;
        --i;
        ++ap;
        ++count;
    }

    a->count = count;
    //cout<<"end sub merge."<<count<<endl;
    return count;
}
} // end namespace bible.

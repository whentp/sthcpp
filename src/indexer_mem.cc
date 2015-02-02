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

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <string.h>
#include "tokenizer_init.h"
#include "structure.h"
#include "file_op.h"
#include "common.h"
#include "file_cache.h" // for file cache.
#include "indexer_mem.h"

namespace bible {
using namespace std;

void addTextToMemIndexHandler(
    const char *keystr,
    const char *valuestr,
    vector<TokenItem> *to,
    Container *fcontainer,
    const char *tokenizer_name) 
{
    const char *raw_string = valuestr;
    size_t length = strlen(raw_string);

    if (!length) return;

    BibleIntType max_file_offset = getfileoffset(MAX_BIBLE_INT_VALUE);
    BibleIntType max_file_number = getfilenumber(MAX_BIBLE_INT_VALUE);

    BibleIntType file_number = fcontainer->GetFileNumber(keystr);

    if (file_number >= max_file_number) {
        cout << "container full. cannot add file." << endl;
        exit(0);
    }

    auto tokenizer = globalTokenizers[tokenizer_name];
    auto hashlist = tokenizer(raw_string);

    BibleIntType processed_token_count = 0;

    for (auto item = hashlist->begin(); item != hashlist->end(); ++item) {
        // donno how to convert item to void*. therefore write an ugly line.
        TokenItem *current_item = &(*item);
        current_item->offset = makeFileNode(file_number, current_item->offset);
        ++processed_token_count;
        to->push_back(*current_item); // here append to the memblock
        if (processed_token_count >= max_file_offset) {
            cout << "Only the first " << processed_token_count << "tokens are indexed." << endl;
            break;
        }
    }
    delete hashlist;
}

size_t sortMemIndex(vector<TokenItem> * mem_index) {
    size_t tmpint = mem_index->size();
    if(!tmpint){
        return 0;
    }
    CompareNode *tmp = (CompareNode *)(&(mem_index->at(0)));
    sort(tmp, tmp + tmpint, compareIndex);
    return tmpint;
}

size_t compressMemIndex(
    vector<TokenItem> * mem_index,
    const char *filename_compress,
    const char *filename_keyindex)
{
    size_t tmpint = mem_index->size();
    size_t currentp = 0, m1;

    KeyNode tmpkeynode;

    //cout << "Compressing index... ";

    TokenItem *tmp = (TokenItem *)(&(mem_index->at(0)));

    fstream compressf(filename_compress, ios::out | ios::binary);
    compressf.seekp(0, ios::beg);

    string keyindexcache = filename_keyindex;
    keyindexcache += file_ext_keyindex_cache;
    if (checkFileExists(keyindexcache.c_str())) {
        remove(keyindexcache.c_str());
    }

    fstream keyf(filename_keyindex, ios::out | ios::binary);
    keyf.seekp(0, ios::beg);

    // file cache.
    size_t cache_size = 4096;
    FileCache fc_keyf(cache_size);
    FileCache fc_compressf(cache_size);
    fc_keyf.Serve(&keyf, 0);
    fc_compressf.Serve(&compressf, 0);

    tmpkeynode.key = 0;
    tmpkeynode.start = 0;
    tmpkeynode.length = 0;
    for (size_t i = 0; i < tmpint; ++i) {
        if (tmpkeynode.key != tmp[i].hash) {
            m1 = fc_compressf.Tellp();
            tmpkeynode.length = m1 - currentp;
            fc_keyf.Write((char *)&tmpkeynode, sizeof(KeyNode));
            tmpkeynode.key = tmp[i].hash;
            tmpkeynode.start = m1;
            currentp = m1;
        }
        fc_compressf.Write((char *) & (tmp[i].offset), sizeof(BibleIntType));
    }
    m1 = fc_compressf.Tellp();
    tmpkeynode.length = m1 - currentp;
    fc_keyf.Write((char *)&tmpkeynode, sizeof(KeyNode));

    fc_keyf.Free();
    fc_compressf.Free();
    compressf.close();
    keyf.close();
    delete[] tmp;
    return tmpint;
}

} // end namespace bible.

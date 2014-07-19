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

#ifndef _KEYINDEX_CACHE_H__
#define _KEYINDEX_CACHE_H__

#include <string>
#include <fstream>
#include "structure.h"
#include "tokenizer.h"
#include "keyword_tree.h"

namespace bible {

class KeyIndex;
class KeyIndexCache;

class KeyIndex {
private:
    ifstream _keyindexf;
    size_t _keynodecount;
    bool _needcache;
    KeyIndexCache *_keyindexcache;

public:
    KeyIndex(const char *fkeyindex): KeyIndex(fkeyindex, true) {}
    KeyIndex(const char *fkeyindex, const bool needcache);
    ~KeyIndex();
    size_t GetCount() {
        return _keynodecount;
    }
    KeyNode ReadSingleKey(const size_t offset); ///< offset is the index, not seekg/seekp.
    KeyNode *ReadKeyBlock(const size_t offset, const size_t count);
    CompareNode Find(const BibleIntType key);
    void SetupCache(KeyIndexCache *keyindexcache);
};

struct KeyIndexCacheNode {
    BibleIntType key;
    size_t offset;
};

class KeyIndexCache {
private:
    string _filename;
    size_t _block_size;
    KeyIndexCacheNode *_mem;
    size_t _mem_size;
    KeyIndex *_keyindex;
public:
    KeyIndexCache(const char *str, const size_t block_size = 1000);
    ~KeyIndexCache();
    /**
     * @brief create cache.
     */
    void UpdateCache();

    /**
     * @brief Read a whole file to mem.
     */
    void LoadToMem();
    void DeleteCache();

    /**
     * @brief set a keyindex for cache to read keys in keyindex.
     * @details [long description]
     *
     * @param keyindex [description]
     */
    void SetKeyIndex(KeyIndex *keyindex);
    bool Exist();

    /**
     * @brief find key in cache and keyindex.
     *
     * @param key the key.
     * @return same to keyindex::find
     */
    CompareNode Find(const BibleIntType key);
};

} // end namespace bible.
#endif

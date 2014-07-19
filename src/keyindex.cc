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
#include <fstream>
#include "file_op.h"
#include "structure.h"
#include "common.h"
#include "keyindex.h"

namespace bible {

KeyIndex::KeyIndex(const char *fkeyindex, const bool needcache) {
    //cout << "open keyindex: " << fkeyindex << endl;
    _keyindexf.open(fkeyindex, ios::in | ios::binary);
    if (!_keyindexf.is_open()) {
        cout << "Unable to open keyindex file" << endl;
        exit(0);
    }
    _keynodecount = getFileLength(fkeyindex) / sizeof(KeyNode);
    _needcache = needcache;
    if (needcache) {
        _keyindexcache = new KeyIndexCache(fkeyindex);
        _keyindexcache->SetKeyIndex(this);
        //SetupCache(_keyindexcache)
    }
}

KeyIndex::~KeyIndex() {
    _keyindexf.close();
    if (_needcache) {
        delete _keyindexcache;
    }
}

void KeyIndex::SetupCache(KeyIndexCache *keyindexcache) {
    _keyindexcache = keyindexcache;
    keyindexcache->SetKeyIndex(this);
    _needcache = true;
}

KeyNode KeyIndex::ReadSingleKey(const size_t offset) {
    KeyNode tmpk;
    _keyindexf.seekg(offset * sizeof(KeyNode), ios::beg);
    _keyindexf.read((char *)(&tmpk), sizeof(KeyNode));
    return tmpk;
}

KeyNode *KeyIndex::ReadKeyBlock(const size_t offset, const size_t count) {
    KeyNode *tmpk = new KeyNode[count];
    //cout << "offset:" << offset << ", count:" << count << endl;
    _keyindexf.seekg(offset * sizeof(KeyNode), ios::beg);
    _keyindexf.read((char *)(tmpk), count * sizeof(KeyNode));
    return tmpk;
}

CompareNode KeyIndex::Find(const BibleIntType key) {
    if (_needcache) {
        return _keyindexcache->Find(key);
    }
    size_t l, m, r; // left, middle, right.
    KeyNode tmpk;
    CompareNode returnvalue;

    returnvalue.n2 = 0;
    returnvalue.n1 = 0;

    l = 0; //1; // here is 1?! not 0?! pls check!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    r = _keynodecount - 1;
    m = (l + r) / 2;

    while (l <= m && m <= r) {
        _keyindexf.seekg(m * sizeof(KeyNode), ios::beg);
        _keyindexf.read((char *)(&tmpk), sizeof(KeyNode));
        if (tmpk.key == key) {
            returnvalue.n1 = tmpk.start;
            returnvalue.n2 = tmpk.length;
            break;
        }
        else if (tmpk.key > key) {
            r = m - 1; m = (l + r) / 2;
        }
        else if (tmpk.key < key) {
            l = m + 1; m = (l + r) / 2;
        }
    }
    return returnvalue;
}

KeyIndexCache::KeyIndexCache(const char *str, const size_t block_size) {
    string tmp = "";
    _filename = tmp + str;
    _block_size = block_size;
    _mem = NULL;
    _mem_size = 0;
    _keyindex = NULL;
    LoadToMem();
}

KeyIndexCache::~KeyIndexCache() {
    if (_mem) {
        delete[] _mem;
        _mem = NULL;
    }
}

bool KeyIndexCache::Exist() {
    return checkFileExists((_filename + file_ext_keyindex_cache).c_str());
}

void KeyIndexCache::DeleteCache() {
    if (Exist()) {
        remove((_filename + file_ext_keyindex_cache).c_str());
    }
}

void KeyIndexCache::SetKeyIndex(KeyIndex *keyindex) {
    _keyindex = keyindex;
}

void KeyIndexCache::UpdateCache() {
    string keyindex_file = _filename;// + file_ext_keyindex;
    string keyindexcache_file = _filename + file_ext_keyindex_cache;

    size_t keyindex_file_size = getFileLength(keyindex_file.c_str()) / sizeof(KeyNode);

    ifstream fkeyindex(keyindex_file.c_str(), ios::in | ios::binary);
    if (!fkeyindex.is_open()) {
        cout << "open keyindex file error. " << keyindex_file << endl;
        exit(0);
    }

    ensureFileExists(keyindexcache_file.c_str());
    ofstream cachefile(keyindexcache_file.c_str(), ios::out | ios::binary);
    if (!cachefile.is_open()) {
        cout << "open keyindexcache file error. " << keyindexcache_file << endl;
        exit(0);
    }

    KeyNode tmpkeynode;
    KeyIndexCacheNode cache_node;
    for (size_t i = 0; i < keyindex_file_size; i += _block_size) {
        fkeyindex.seekg(i * sizeof(KeyNode), ios::beg);
        fkeyindex.read((char *)&tmpkeynode, sizeof(KeyNode));
        cache_node.key = tmpkeynode.key;
        cache_node.offset = i;
        cachefile.write((char *)&cache_node, sizeof(KeyIndexCacheNode));
    }
    if (cache_node.offset + 1 < keyindex_file_size) {
        fkeyindex.seekg((keyindex_file_size - 1) * sizeof(KeyNode), ios::beg);
        fkeyindex.read((char *)&tmpkeynode, sizeof(KeyNode));
        cache_node.key = tmpkeynode.key;
        cache_node.offset = keyindex_file_size - 1;
        cachefile.write((char *)&cache_node, sizeof(KeyIndexCacheNode));
    }
    cachefile.close();
    fkeyindex.close();
}

void KeyIndexCache::LoadToMem() {
    string keyindexcache_file = _filename + file_ext_keyindex_cache;
    // here should check if file exists.
    if (getFileLength(keyindexcache_file.c_str()) == 0) {
        UpdateCache();
    }
    loadFile(keyindexcache_file.c_str(), (char *&)_mem, _mem_size); // is the "&" correct?
    _mem_size /= sizeof(KeyIndexCacheNode);
}

CompareNode KeyIndexCache::Find(const BibleIntType key) {
    if (!_keyindex) {
        throw "keyindex is not prepared.";
    }
    CompareNode returnvalue;
    returnvalue.n2 = 0;
    returnvalue.n1 = 0;
    //should here check if mem exists?
    if (key < _mem[0].key || key > _mem[_mem_size - 1].key) {
        return returnvalue;
    }

    size_t l, m, r; // left, middle, right.
    KeyIndexCacheNode tmpk;

    l = 0;
    r = _mem_size - 1;
    m = (l + r) / 2;

    while (l <= m && m <= r) {
        tmpk = _mem[m];
        //cout << tmpk.key << "," << key << "\tl:" << l << ";\tm:" << m << ";\tr" << r << endl;
        if (tmpk.key == key) {
            KeyNode tmpkeynode;
            tmpkeynode = _keyindex->ReadSingleKey(tmpk.offset);
            returnvalue.n1 = tmpkeynode.start;
            returnvalue.n2 = tmpkeynode.length;
            return returnvalue;
        }
        else if (tmpk.key > key) {
            r = m - 1; m = (l + r) / 2;
        }
        else if (tmpk.key < key) {
            l = m + 1; m = (l + r) / 2;
        }
    }

    //if not found, pick up the smallest from l, m.
    m = (m < l) ? m : l; // is this safe?
    //this is not needed. m = (m < r)? m : r;

    //cout << _mem[m].key << "\tl:" << l << ";\tm:" << m << ";\tr:" << r << endl;
    size_t readcount = _keyindex->GetCount() - _mem[m].offset; //m?! wrong.
    if (readcount > _block_size) {
        readcount = _block_size;
    }

    KeyNode *keynode_block = _keyindex->ReadKeyBlock(_mem[m].offset, readcount);

    l = 0;
    r = readcount - 1;
    m = (l + r) / 2;

    while (l <= m && m <= r) {
        //cout << keynode_block[m].key << "," << key << "\tl:" << l << ";\tm:" << m << ";\tr" << r << endl;
        if (keynode_block[m].key == key) {
            returnvalue.n1 = keynode_block[m].start;
            returnvalue.n2 = keynode_block[m].length;
            break;
        }
        else if (keynode_block[m].key > key) {
            r = m - 1; m = (l + r) / 2;
        }
        else if (keynode_block[m].key < key) {
            l = m + 1; m = (l + r) / 2;
        }
    }
    return returnvalue;
}

} // end namespace bible.


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
#include <fstream>
#include <algorithm>
#include <string.h>
#include "tokenizer_init.h"
#include "structure.h"
#include "file_op.h"
#include "common.h"
#include "container.h"
#include "file_cache.h" // for file cache.

namespace bible {
using namespace std;

void addTextToIndex(
    const char *keystr,
    const char *valuestr,
    const char *to,
    const char *fcontainer)
{
    const char *raw_string = valuestr;
    size_t length = strlen(raw_string);

    if (!length) return; // should here return??? when length == 0, should i create some empty files?

    Container tmpcontainer(fcontainer);

    BibleIntType max_file_offset = getfileoffset(MAX_BIBLE_INT_VALUE);
    BibleIntType max_file_number = getfilenumber(MAX_BIBLE_INT_VALUE);

    BibleIntType file_number = tmpcontainer.GetFileNumber(keystr);

    if (file_number >= max_file_number) {
        cout << "container full. cannot add file." << endl;
        exit(0);
    }
    //cout << file_number << endl;
    ensureFileExists(to);

    string tokenizer_name = config_default_tokenizer;
    tokenizer_name = globalConfigs.Read("tokenizer", tokenizer_name);
    auto tokenizer = globalTokenizers[tokenizer_name];
    auto hashlist = tokenizer(raw_string);

    BibleIntType processed_token_count = 0;

    for (auto item = hashlist->begin(); item != hashlist->end(); ++item) {
        // donno how to convert item to void*. therefore write an ugly line.
        TokenItem *current_item = &(*item);
        current_item->offset = makeFileNode(file_number, current_item->offset);
        ++processed_token_count;
        if (processed_token_count >= max_file_offset) {
            cout << "Only the first " << processed_token_count << "tokens are indexed." << endl;
            break;
        }
    }

    if (hashlist->size()) {
        fstream file (to, ios::in | ios::out | ios::binary | ios::ate);
        if (file.is_open()) {
            file.write((const char *)(&((*hashlist)[0])), sizeof(TokenItem) * processed_token_count);
            file.close();
            //here should output some information.
            //cout << "Processing " << keystr
            //<< ", length: " << length << " bytes, ratio: "
            //<< (double)processed_token_count * sizeof(TokenItem) / length << endl;
        }
    }
    //cout << "size:" << hashlist->size() << endl;
    delete hashlist;
    //any leak?
}

void addFileToIndex(
    const char *filename,
    const char *to,
    const char *fcontainer)
{
    char *raw_string = NULL;
    size_t filelength;
    loadTextFile(filename, raw_string, filelength);
    addTextToIndex(filename, raw_string, to, fcontainer);
    delete[] raw_string;
}

size_t sortIndex(const char *filename) {
    size_t filelength = getFileLength(filename);
    size_t tmpint = filelength / sizeof(CompareNode);
    FILE *filenameindex = fopen(filename, "rb+"); // errr... i use c here just because i donno the alternative for "rb+" in c++.
    auto tmp = new CompareNode[tmpint];

    //cout << "Sorting index... ";
    fseek(filenameindex, 0, SEEK_SET);
    rewind(filenameindex);
    fread(tmp, sizeof(CompareNode), tmpint, filenameindex);
    sort(tmp, tmp + tmpint, compareIndex);
    fseek(filenameindex, 0, SEEK_SET);
    rewind(filenameindex);
    fwrite(tmp, sizeof(CompareNode), tmpint, filenameindex);
    fclose(filenameindex);
    //cout << "OK." << endl;

    delete[] tmp;
    return tmpint;
}

size_t compressIndex(
    const char *filename_raw,
    const char *filename_compress,
    const char *filename_keyindex)
{
    size_t filelength = getFileLength(filename_raw);
    size_t tmpint = filelength / sizeof(CompareNode);
    size_t currentp = 0, m1;
    //FILE* compressf;
    //FILE* keyf;
    KeyNode tmpkeynode;

    //cout << "Compressing index... ";

    TokenItem *tmp = new TokenItem[tmpint];
    ifstream filenameindex(filename_raw, ios::in | ios::binary);
    //fseek(filenameindex,0,SEEK_SET);
    filenameindex.read((char *)tmp, filelength);
    filenameindex.close();

    fstream compressf(filename_compress, ios::out | ios::binary);
    compressf.seekp(0, ios::beg);

    //rewind(compressf);

    //remove keyindexcache.
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
    remove(filename_raw);
    //cout << "OK." << endl;
    return tmpint;
}

void mergeIndex(
    const char *container1,
    const char *container2,
    const char *tmpcontainer,
    const char *keyindex1,
    const char *keyindex2,
    const char *tmpkeyindex,
    const char *compressed1,
    const char *compressed2,
    const char *tmpcompressed)
{
    /*cout << container1 << endl;
    cout << container2 << endl;
    cout << keyindex1 << endl;
    cout << keyindex2 << endl;
    cout << compressed1 << endl;
    cout << compressed2 << endl;*/

    size_t len1 = getFileLength(container1);
    size_t len2 = getFileLength(container2);

    //remove keyindexcache.
    string keyindexcache = keyindex1;
    keyindexcache += file_ext_keyindex_cache;
    if (checkFileExists(keyindexcache.c_str())) {
        remove(keyindexcache.c_str());
    }
    keyindexcache = keyindex2;
    keyindexcache += file_ext_keyindex_cache;
    if (checkFileExists(keyindexcache.c_str())) {
        remove(keyindexcache.c_str());
    }

    // merge two container. simple.

    Container container(container1);

    len1 = container.Merge(container2);
    container.Close();

    string tmp1 = file_ext_container_key;//".container";
    rename((container1 + tmp1).c_str(), (tmpcontainer + tmp1).c_str());
    string tmp2 = file_ext_container_value; //".barnvalue";
    rename((container1 + tmp2).c_str(), (tmpcontainer + tmp2).c_str());

    BibleIntType file_number_offset = makeFileNode(len1, 0);
    // a little dangder. generate an offset used for calculate new file numbers.

    fstream newkey(tmpkeyindex, ios::out | ios::binary);
    fstream newcompressed(tmpcompressed, ios::out | ios::binary);
    len1 = getFileLength(keyindex1);
    len2 = getFileLength(keyindex2);
    fstream fk1(keyindex1, ios::in | ios::binary);
    fstream fk2(keyindex2, ios::in | ios::binary);
    fstream fc1(compressed1, ios::in | ios::binary);
    fstream fc2(compressed2, ios::in | ios::binary);
    if (!fk1.is_open()) throw "error";
    if (!fk2.is_open()) throw "error";
    if (!fc1.is_open()) throw "error";
    if (!fc2.is_open()) throw "error";

    // cache for reading
    size_t cache_size = 4096;
    FileCache fc_fk1(cache_size);
    FileCache fc_fk2(cache_size);
    FileCache fc_fc1(cache_size);
    FileCache fc_fc2(cache_size);
    fc_fk1.Serve(&fk1, len1);
    fc_fk2.Serve(&fk2, len2);
    fc_fc1.Serve(&fc1, getFileLength(compressed1));
    fc_fc2.Serve(&fc2, getFileLength(compressed2));

    // cache for writing
    FileCache fc_newkey(cache_size);
    FileCache fc_newcompressed(cache_size);


    fc_newkey.Serve(&newkey, 0);
    fc_newcompressed.Serve(&newcompressed, 0);

    size_t i = 0, j = 0;

    KeyNode tmpk1, tmpk2;
    //hope all keys can be read to memory.
    fc_fk1.Read((char *)&tmpk1, sizeof(KeyNode));
    fc_fk2.Read((char *)&tmpk2, sizeof(KeyNode));

    char *b1, *b2;

    len1 /= sizeof(KeyNode);
    len2 /= sizeof(KeyNode);
    while (i < len1 || j < len2) {
        //cout << "i="<< i << "/" << len1 << "\tj="<< j << "/" << len2 << endl;
        if (tmpk1.key < tmpk2.key) {
            if (tmpk1.length > 0) {
                b1 = new char[tmpk1.length];
                fc_fc1.Read(b1, tmpk1.length);
                tmpk1.start = fc_newcompressed.Tellp();
                fc_newcompressed.Write(b1, tmpk1.length);
                fc_newkey.Write((char *)&tmpk1, sizeof(KeyNode));
                delete[] b1;
            }
            ++i;
            if (i < len1) {
                fc_fk1.Read((char *)&tmpk1, sizeof(KeyNode));
            } else {
                tmpk1.key = MAX_BIBLE_INT_VALUE;//-1; // is it safe? or UINT_MAX?
            }
        } else if (tmpk1.key > tmpk2.key) {
            if (tmpk2.length > 0) {
                b2 = new char[tmpk2.length];
                fc_fc2.Read(b2, tmpk2.length);

                // the following section... urrrr... how can remove it when we do not need it.
                // e.g. when every file_number is already unique.
                size_t count = tmpk2.length / sizeof(BibleIntType);
                BibleIntType *tmpadder = (BibleIntType *)(b2);
                while (count-- > 0) {
                    (*tmpadder) += file_number_offset;
                    ++tmpadder;
                }

                // add sth to b2!!!!!!
                tmpk2.start = fc_newcompressed.Tellp();
                fc_newcompressed.Write(b2, tmpk2.length);
                fc_newkey.Write((char *)&tmpk2, sizeof(KeyNode));
                delete[] b2;
            }
            ++j;
            if (j < len2) {
                fc_fk2.Read((char *)&tmpk2, sizeof(KeyNode));
            } else {
                tmpk2.key = MAX_BIBLE_INT_VALUE;//-1; // is it safe? or UINT_MAX?
            }
        } else {
            if ((tmpk1.length + tmpk2.length) > 0) {
                b1 = new char[tmpk1.length + tmpk2.length];

                if (tmpk1.length > 0) {
                    fc_fc1.Read(b1, tmpk1.length);
                }
                if (tmpk2.length) {
                    fc_fc2.Read(b1 + tmpk1.length, tmpk2.length);
                }
                // do sth to the 2nd block.
                // same to aforehead. how to remove it, because of the flexibility.
                size_t count = tmpk2.length / sizeof(BibleIntType);
                BibleIntType *tmpadder = (BibleIntType *)(b1 + tmpk1.length);
                while (count-- > 0) {
                    (*tmpadder) += file_number_offset;
                    ++tmpadder;
                }
                //need to sort? maybe no.
                tmpk1.start = fc_newcompressed.Tellp();
                tmpk1.length += tmpk2.length;
                fc_newcompressed.Write(b1, tmpk1.length);
                fc_newkey.Write((char *)&tmpk1, sizeof(KeyNode));

                delete[] b1;
            }
            ++i; ++j;
            // errr... make sure all i and j are processed.
            if (i < len1) {
                fc_fk1.Read((char *)&tmpk1, sizeof(KeyNode));
            } else {
                tmpk1.key = MAX_BIBLE_INT_VALUE;//-1; // is it safe? or UINT_MAX?
            }
            if (j < len2) {
                fc_fk2.Read((char *)&tmpk2, sizeof(KeyNode));
            } else {
                tmpk2.key = MAX_BIBLE_INT_VALUE;//-1; // is it safe? or UINT_MAX?
            }
        }
    }
    // really? is that ok???

    // file_cache should be released before file is closed.
    fc_fk1.Free();
    fc_fk2.Free();
    fc_fc1.Free();
    fc_fc2.Free();
    fc_newcompressed.Free();
    fc_newkey.Free();

    newkey.close();
    newcompressed.close();
    fk1.close();
    fk2.close();
    fc1.close();
    fc1.close();
}

} // end namespace bible.

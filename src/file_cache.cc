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

#include <algorithm>
#include <iostream>
#include "file_cache.h"

namespace bible {

using namespace std;

FileCache::FileCache(size_t cache_size) {
    _cache_size = cache_size;
}

FileCache::~FileCache() {
    Free();
}

void FileCache::Serve(fstream *fs, const size_t filesize) {
    _fs = fs;
    _mem = new char[_cache_size];
    _mem_offset = -1; // ok?
    _real_file_size = filesize;
}

void FileCache::Free() {
    Commit();
    if (_mem) {
        delete[] _mem;
        _mem = nullptr;
        _cache_size = 0;
    }
    _fs = nullptr;
}

void FileCache::Commit() {
    if (_mem_offset && _mem_offset <= _cache_size) {
        //cout << "Real Writing (in byte): " << _mem_offset << endl;
        _fs->write(_mem, _mem_offset);
        _mem_offset = 0;
        _the_real_p = _fs->tellp();
    }
}

void FileCache::Seekg(const size_t pos, ios_base::seekdir way) {
    /*_g = pos;
    if ((_the_real_g < pos) && (pos < _the_real_g + _cache_size)) {
        _mem_offset = pos - _the_real_g;
    } else {
        _mem_offset = 0;
        _the_real_g = pos;
        size_t readsize = _real_file_size - _the_real_g;

        readsize = (readsize > _cache_size) ? _cache_size : readsize;
        _fs->seekg(pos, ios_base::beg);
        _fs->read(_mem, readsize);
    }*/
}

size_t FileCache::Tellg() {
    return _the_real_prev_g + _mem_offset;
}

void FileCache::Seekp(size_t pos, ios_base::seekdir way) {

}

size_t FileCache::Tellp() {
    if (_mem_offset > _cache_size) _mem_offset = 0;
    return _the_real_p + _mem_offset;
}

void FileCache::ReadBlockFromFile(char *block, size_t size_limit) {
    size_t readsize = _real_file_size - _fs->tellg();
    readsize = (readsize > _cache_size) ? size_limit : readsize;
    //cout << "Real Reading (in byte): " << readsize << endl;
    _the_real_prev_g = _fs->tellg();
    _fs->read(block, readsize);
    _the_real_g = _fs->tellg();
}

void FileCache::ReadBlockFromFile() {
    ReadBlockFromFile(_mem, _cache_size);
    _mem_offset = 0;
}

void FileCache::Read(char *s, const size_t n) {
    if (n == 0) return;
    if (_mem_offset >= _cache_size) {
        ReadBlockFromFile();
        _mem_offset = 0;
    }

    if (_mem_offset + n < _cache_size) {
        copy(_mem + _mem_offset, _mem + _mem_offset + n, s);
        _mem_offset += n;
    } else {
        size_t remaining_size = n - (_cache_size - _mem_offset);
        copy(_mem + _mem_offset, _mem + _cache_size, s);
        _mem_offset = -1;

        if (remaining_size > _mem_offset) {
            ReadBlockFromFile(s + (n - remaining_size), remaining_size);
        } else {
            ReadBlockFromFile();
            copy(_mem, _mem + remaining_size, s + (n - remaining_size));
            _mem_offset = remaining_size;
        }
    }
}

void FileCache::Write(char *s, const size_t n) {
    if (n == 0) return;
    size_t remaining_size = n;
    if (_mem_offset > _cache_size) {
        _mem_offset = 0;
    }
    if (remaining_size >= _cache_size) {
        Commit();
        _fs->write(s, n);
        _the_real_p = _fs->tellp();
    } else {
        size_t handle_size = _cache_size - _mem_offset;
        if (handle_size > remaining_size) {
            handle_size = remaining_size;
            remaining_size = 0;
        } else {
            remaining_size -= handle_size;
        }

        copy(s, s + handle_size, _mem + _mem_offset);
        _mem_offset += handle_size;
        if (_mem_offset == _cache_size) {
            Commit();
        }
        if (remaining_size) {
            copy(s + handle_size, s + n, _mem + _mem_offset);
            _mem_offset += remaining_size;
        }
    }
}

#ifdef _TEST_

void testFileCache() {
    string test_file_name = "test_file_cache";
    fstream writer(test_file_name, ios::out | ios::binary);
    if (writer.is_open()) {
        FileCache *fc = new FileCache(1000000);
        fc->Serve(&writer, 0);
        for (size_t i = 0; i < 100; ++i) {
            fc->Write((char *)&i, sizeof(size_t));
            fc->Write(((char *)&i) + 1, sizeof(size_t) - 1);
            cout << i << ", tellp: " << fc->Tellp() << endl;
        }
        delete fc;
        writer.close();
    }

    fstream reader(test_file_name, ios::in | ios::binary);
    if (reader.is_open()) {
        FileCache *fc = new FileCache(100000);
        fc->Serve(&reader, getFileLength(test_file_name.c_str()));
        for (size_t i = 0; i < 100; ++i) {
            size_t tmp;
            fc->Read((char *)&tmp, sizeof(size_t));
            cout << i << "\t->\t" << tmp << ", tellg: " << fc->Tellg() << endl;
            fc->Read(((char *)&tmp) + 1, sizeof(size_t) - 1);
        }
        delete fc; // should be deleted before file is closed.
        reader.close();
    }
}

#endif


} // end namespace bible

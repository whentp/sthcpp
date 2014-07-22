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
    _mem_offset = 0;
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
    if (_mem_offset) {
        _fs->write(_mem, _mem_offset);
        _mem_offset = 0;
    }
}

void FileCache::Seekg(const size_t pos, ios_base::seekdir way) {
    _g = pos;
    if ((_the_real_g < pos) && (pos < _the_real_g + _cache_size)) {
        _mem_offset = pos - _the_real_g;
    } else {
        _mem_offset = 0;
        _the_real_g = pos;
        size_t readsize = _real_file_size - _the_real_g;

        readsize = (readsize > _cache_size) ? _cache_size : readsize;
        _fs->seekg(pos, ios_base::beg);
        _fs->read(_mem, readsize);

    }

}

size_t FileCache::Tellg() {
    return 0;
}

void FileCache::Seekp(size_t pos, ios_base::seekdir way) {

}

size_t FileCache::Tellp() {
    return 0;
}

void FileCache::Read(char *s, const size_t n) {
    if (n == 0) return;
    if (_mem_offset + n < _cache_size) {
        copy(_mem + _mem_offset, _mem + _mem_offset + n, s);
        _mem_offset += n;
    } else {
        size_t remaining_size = n - (_cache_size - _mem_offset);
        copy(_mem + _mem_offset, _mem + _cache_size, s);
        _mem_offset = 
    }
}

void FileCache::Write(char *s, const size_t n) {
    if (n == 0) return;
    size_t remaining_size = n;
    if (remaining_size >= _cache_size) {
        Commit();
        _fs->write(s, n);
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

} // end namespace bible

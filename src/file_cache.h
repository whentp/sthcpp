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

#ifndef FILE_CACHE_H
#define FILE_CACHE_H

#include <fstream>

namespace bible {

using namespace std;

class FileCache{
private:
	fstream *_fs = nullptr;
	char *_mem = nullptr;
	size_t _mem_offset = 0;
	size_t _cache_size = 0;
	size_t _g = -1;
	size_t _the_real_g = -1;
	size_t _p = 0;
	size_t _the_real_p = 0;
	size_t _real_file_size = 0;
public:
	FileCache(size_t cache_size);
	~FileCache();
	void Serve(fstream *fs, const size_t filesize);
	void Free();
	void Commit();
	void Seekg(const size_t pos, ios_base::seekdir way);
	size_t Tellg();
	void Seekp(size_t pos, ios_base::seekdir way);
	size_t Tellp();
	void Read(char* s, const size_t n);
	void Write(char *s, const size_t n);
};

} // end namespace bible.

#endif

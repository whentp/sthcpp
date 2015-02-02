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

#ifndef _INDEXER_MEM_H_
#define _INDEXER_MEM_H_

#include <vector>
#include "structure.h"
#include "common.h"
#include "container.h"

namespace bible {
using namespace std;

void addTextToMemIndexHandler(
    const char *keystr,
    const char *valuestr,
    vector<TokenItem> *to,
    Container *fcontainer,
    const char *tokenizer_name);

size_t sortMemIndex(vector<TokenItem> *mem_index);

size_t compressMemIndex(
    vector<TokenItem> *mem_index,
    const char *filename_compress,
    const char *filename_keyindex);

} // end namespace bible.

#endif
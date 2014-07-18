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

#include <string>
#include <time.h>
#include <sstream>
#include "common.h"

namespace bible {

StopWatch::StopWatch() {
    start = clock();
}

double StopWatch::Stop() {
    double duration = (double)(clock() - start) / CLOCKS_PER_SEC;
    return duration;
}

// just donno why it cannot work (output undefined symbol when linking). bug of compiler?
template <typename T>
string NumberToString(T Number) {
    ostringstream ss;
    ss << Number;
    return ss.str();
}

template <typename T>
T StringToNumber(const string &str) {
    istringstream ss(str);
    T result;
    return ss >> result ? result : 0;
}

string size_t_to_string(size_t number) {
    ostringstream ss;
    ss << number;
    return ss.str();
}

const char *configuration_filename = "config.txt";
const char *config_default_tokenizer = "english";

const char *file_ext_container_key = ".container";
const char *file_ext_container_value = ".barnvalue";
const char *file_ext_keyindex = ".keyindex";
const char *file_ext_compressedindex = ".compressedindex";
const char *file_ext_keyindex_cache = ".keyindexcache";
const char *file_fixed_container_prefix = "fixed_container_";

} // end namespace bible

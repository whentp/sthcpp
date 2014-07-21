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
#include <sstream>
#include <algorithm>
#include <chrono>
#include "misc.h"

namespace bible {

StopWatch::StopWatch() {
    start = std::chrono::high_resolution_clock::now();
}

void StopWatch::Start() {
    start = std::chrono::high_resolution_clock::now();
}

double StopWatch::Stop() {
    std::chrono::system_clock::time_point endingtime = std::chrono::high_resolution_clock::now();
    double duration = (double)(std::chrono
                               ::duration_cast<std::chrono::milliseconds>(endingtime - start).count()) / 1000;
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

string toLowercase(const string &original) {
    string data = original;
    transform(data.begin(), data.end(), data.begin(), ::tolower);
    return data;
}

} // end namespace bible

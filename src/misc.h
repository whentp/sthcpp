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

#ifndef MISC_H
#define MISC_H

#include <string>
#include <sstream>
#include <chrono> // for StopWatch.

namespace bible {

using namespace std;

class StopWatch {
private:
    std::chrono::system_clock::time_point start;
public:
    StopWatch();
    void Start();
    double Stop();
};

// WHY to_string does not work!!!!!!!!!!!
// just donno why it cannot work (output undefined symbol when linking). bug of compiler?
template <typename T>
string NumberToString(T Number);

template <typename T>
T StringToNumber(const string &str);

string size_t_to_string(size_t);

string toLowercase(const string &original);

} // end namespace bible.

#endif

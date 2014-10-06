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

#ifndef _KEY_VALUE_CONTAINER_H_
#define _KEY_VALUE_CONTAINER_H_

#include <string>
#include <vector>
#include "schedule.h"

namespace bible {

class KeyValueContainer;

class KeyValueContainer {
private:
    string _path;
    Schedule *_schedule;
public:
    KeyValueContainer(const std::string &path);
    ~KeyValueContainer();
    void AddKeyValue(const char *key, const char *value);
    void Commit();
    size_t GetValues(const char *key, std::vector<std::string> *values);
};

} // end namespace bible

#endif
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

#include "key_value_container.h"
#include "file_op.h"
#include "structure.h"

namespace bible {

using namespace std;

KeyValueContainer::KeyValueContainer(const string &path) {
    _path = path;
    if (!folder_exists(path)) {
        _mkdir(path.c_str());
    }

    string tokenizer_name = "singleton";
    _schedule = new Schedule(_path, tokenizer_name, 0);
    _schedule->PrepareSearchers();
}

KeyValueContainer::~KeyValueContainer() {
    delete _schedule;
}

void KeyValueContainer::AddKeyValue(const char *key, const char *value) {
    string filename;
    _schedule->AddText(value, key);
}

void KeyValueContainer::Commit() {
    _schedule->Commit();
    _schedule->PrepareSearchers();
}

size_t KeyValueContainer::GetValues(const char *key, vector<string> *values) {
    SearchResult *res = _schedule->Search(key);
    if (!values) {
        values = new vector<string>;
    }
    for (size_t i = 0; i < res->count; ++i) {
        values->push_back(res->filenames[i]);
    }
    if (!res->state) {
        cout << res->msg << endl;
    }
    delete res;
    return values->size();
}

} // end namespace bible
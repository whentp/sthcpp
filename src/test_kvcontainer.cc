#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "key_value_container.h"
#include "test_kvcontainer.h"
#include "common.h"

namespace bible {

using namespace std;


map<string, string> *generateTestDataKv(size_t count) {
    map<string, string> *res = new map<string, string>();
    for (size_t i = 0; i < count; ++i) {
        string k = generateRandStringKv((rand() % 40) + 20);
        string v = generateRandStringKv((rand() % 40) + 400);
        res->insert(pair<string, string>(k, v));
    }
    return res;
}

string generateRandStringKv(size_t size) {
    static char char_used[] = "abcdefghijklmnopqrstuvwxyz0123456789    "; // spaces are necessary. ha.
    string res = "";
    size_t char_used_count = sizeof(char_used) - 1; // -1 means remove the last \0. remember!!!
    for (size_t i = 0; i < size; ++i) {
        res += char_used[rand() % char_used_count];
    }
    return res;
}

void singleKvTestCase(size_t size) {
    auto test_data = generateTestDataKv(size);
    auto container = new KeyValueContainer("test_kv");

    for (auto &i : *test_data) {
        container->AddKeyValue(i.first.c_str(), i.second.c_str());
    }
    container->Commit();
    StopWatch watch;
    for (auto &i : *test_data) {
        auto found = new vector<string>;
        string key = i.first;
        if (rand() % 2) {
            key += "_x";
        }
        if (container->GetValues(key.c_str(), found)) {
            for (auto &j : *found) {
                cout << "found: " << key << endl;//", " << j << endl;
            }
        } else {
            cout << "not found" << endl;
        }
        delete found;
    }
    auto etime = watch.Stop();
    cout << "Test Performance: " << etime << "second(s). " << endl;
    delete test_data;
}

void testKvcontainerPerformance() {
    StopWatch watch;

    singleKvTestCase(50000);

    auto etime = watch.Stop();
    cout << "Test Performance: " << etime << "second(s). " << endl;
}


} // end namespace bible.
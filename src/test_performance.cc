#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "common.h"
#include "schedule.h"
#include "test_performance.h"

namespace bible {

using namespace std;
char char_used[] = "abcdefghijklmnopqrstuvwxyz0123456789    "; // spaces are necessary. ha.

map<string, string> *generateTestData(size_t count) {
    map<string, string> *res = new map<string, string>();
    for (size_t i = 0; i < count; ++i) {
        string k = generateRandString((rand() % 40) + 40);
        string v = generateRandString((rand() % 40) + 40);
        res->insert(pair<string, string>(k, v));
    }
    return res;
}

string generateRandString(size_t size) {
    string res = "";
    size_t char_used_count = sizeof(char_used) - 1; // -1 means remove the last \0. remember!!!
    for (size_t i = 0; i < size; ++i) {
        res += char_used[rand() % char_used_count];
    }
    return res;
}

void testInsertPerformance() {
    StopWatch watch;
    double etime;

    /* initialize random seed: */
    srand (time(NULL));
    watch.Start();
    size_t count = 1000;
    map<string, string> *test_data = generateTestData(count);
    etime = watch.Stop();
    cout << "Test data generated: " << etime << "second(s)." << endl;

    Schedule *indexer = new Schedule("./");

    watch.Start();
    for (auto &i : *test_data) {
        indexer->Start();
        indexer->AddText(i.second.c_str(), i.first.c_str());
        indexer->Commit(); // what if just commit after do nothing?
    }

    etime = watch.Stop();
    cout << "Test Insert Performance: "
         << etime << "second(s). "
         << (double)count / etime << " records per second." << endl;

    delete indexer;
    delete test_data;
}


} // end namespace bible.
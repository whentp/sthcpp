#ifndef TEST_KVCONTAINER_H_
#define TEST_KVCONTAINER_H_

#include <map>
#include <string>

namespace bible {

using namespace std;

void singleKvTestCase(size_t size);
void testKvcontainerPerformance();

string generateRandStringKv(size_t size);
map<string, string> *generateTestDataKv(size_t count) ;

} // end namespace bible.

#endif

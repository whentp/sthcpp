#ifndef TEST_PERFORMANCE_H
#define TEST_PERFORMANCE_H

#include <map>
#include <string>

namespace bible {

map<string, string> *generateTestData(size_t count);
string generateRandString(size_t size);
void testInsertPerformance();

} // end namespace bible.

#endif

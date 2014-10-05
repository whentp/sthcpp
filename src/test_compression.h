#ifndef TEST_COMPRESSION_H
#define TEST_COMPRESSION_H

#include <map>
#include <string>

namespace bible {

void singleTestCase(size_t size);
void singleOrderedTestCase(size_t size);
void singleOrderedUint32TestCase(size_t size);
void testCompressionPerformance();

} // end namespace bible.

#endif

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "common.h"
#include "schedule.h"
#include "test_compression.h"
#include <lzf.h>
#include <algorithm>

namespace bible {

    using namespace std;

    void singleTestCase(size_t size){
        size_t *buffer = new size_t[size];
        for(size_t i = 0; i < size; ++i){
            buffer[i] = rand();
        }

        char *compress_buffer = new char[size * sizeof(size_t)];

        int output_size = lzf_compress (
            buffer,
            size * sizeof(size_t),
            compress_buffer,
            size * sizeof(size_t)*2);

        cout << "in_size: " << size * sizeof(size_t) 
        << ", out_size: " << output_size 
        << ", ratio: " << (double)output_size / (size * sizeof(size_t)) << endl;
    }

    void singleOrderedUint32TestCase(size_t size){
        uint32_t *buffer = new uint32_t[size];
        for(uint32_t i = 0; i < size; ++i){
            buffer[i] = (i % 65536) + (i / 65536 /2) * 65536;
        }

        char *compress_buffer = new char[size * sizeof(uint32_t)];

        sort(buffer, buffer + size);

        int output_size = lzf_compress (
            buffer,
            size * sizeof(uint32_t),
            compress_buffer,
            size * sizeof(uint32_t)*4);

        cout << "in_size: " << size * sizeof(uint32_t) 
        << ", out_size: " << output_size 
        << ", ratio: " << (double)output_size / (size * sizeof(uint32_t)) << endl;
    }

    void singleOrderedTestCase(size_t size){
        ushort *buffer = new ushort[size];
        for(ushort i = 0; i < size; ++i){
            buffer[i] = i;
        }

        char *compress_buffer = new char[size * sizeof(short)];

        int output_size = lzf_compress (
            buffer,
            size * sizeof(short),
            compress_buffer,
            size * sizeof(short)*2);

        cout << "in_size: " << size * sizeof(short) 
        << ", out_size: " << output_size 
        << ", ratio: " << (double)output_size / (size * sizeof(short)) << endl;
    }

    void testCompressionPerformance() {
        StopWatch watch;
        double etime;
    /* initialize random seed: */
        srand(time(NULL));
        watch.Start();

        singleTestCase(1);
        singleTestCase(10);
        singleTestCase(100);
        singleTestCase(1000);
        singleTestCase(10000);
        singleTestCase(100000);
        singleTestCase(1000000);

                singleOrderedUint32TestCase(1);
        singleOrderedUint32TestCase(10);
        singleOrderedUint32TestCase(100);
        singleOrderedUint32TestCase(1000);
        singleOrderedUint32TestCase(10000);
        singleOrderedUint32TestCase(100000);
        singleOrderedUint32TestCase(1000000);

        singleOrderedTestCase(1);
        singleOrderedTestCase(10);
        singleOrderedTestCase(100);
        singleOrderedTestCase(1000);
        singleOrderedTestCase(10000);
        singleOrderedTestCase(100000);
        singleOrderedTestCase(1000000);

        etime = watch.Stop();
        cout << "Test Performance: "
        << etime << "second(s). " << endl;
    }


} // end namespace bible.
#include <time.h>
#include "common.h"

namespace bible{

	StopWatch::StopWatch(){
		start = clock();
	}

	double StopWatch::Stop(){
		double duration = (double)(clock() - start)/CLOCKS_PER_SEC;
		return duration;
	}

	const char *file_ext_container_key = ".container";
	const char *file_ext_container_value = ".barnvalue";
	const char *file_ext_keyindex = ".keyindex";
	const char *file_ext_compressedindex = ".compressedindex";

} // end namespace bible
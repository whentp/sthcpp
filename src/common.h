#ifndef COMMON_H
#define COMMON_H

#include "debug.h"

namespace bible{
	using namespace std;
	class StopWatch {
	private:
		clock_t start;
	public:
		StopWatch();
		double Stop();
	};

// file extensions.
	extern const char *file_ext_container_key;
	extern const char *file_ext_container_value;
	extern const char *file_ext_keyindex;
	extern const char *file_ext_compressedindex;

} // end namespace bible.

#endif

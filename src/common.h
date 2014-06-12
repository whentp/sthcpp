#ifndef COMMON_H
#define COMMON_H

#include "debug.h"

namespace bible{

	class StopWatch {
		private:
			clock_t start;
		public:
			StopWatch();
			double Stop();
	};
} // end namespace bible.
#endif

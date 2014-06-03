#ifndef COMMON_H
#define COMMON_H

#include "debug.h"

class StopWatch {
private:
	clock_t start;
public:
	StopWatch();
	double Stop();
};

#endif

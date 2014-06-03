#include <time.h>
#include "common.h"

StopWatch::StopWatch(){
	start = clock();
}

double StopWatch::Stop(){
	double duration = (double)(clock() - start)/CLOCKS_PER_SEC;
	return duration;
}
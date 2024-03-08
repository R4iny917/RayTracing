#ifndef RANDOMH
#define RANDOMH
#include<stdlib.h>
#include <cstdlib>
#include <ctime>

float drand48(){
	return rand() / (float)(RAND_MAX + 1.0);
}
#endif
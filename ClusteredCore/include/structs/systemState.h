#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H
#include "type3.h"

namespace PSim {

struct systemState {
	int nParticles;
	double concentration;
	int boxSize;
	int cellSize;
	int cellScale;
	double temp;
	double currentTime;
	double dTime;
	int seed;
	int outputFreq;
	double endTime;
};

}
#endif // SYSTEM_STATE_H

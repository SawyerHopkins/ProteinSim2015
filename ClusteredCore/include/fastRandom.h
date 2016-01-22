#ifndef FASTRANDOM_H
#define FASTRANDOM_H
#include "interfaces/IGrid.h"

namespace PSim {

class fastRandom {

private:
	const int ndim = 55;
	const int m10 = 1000000000;
	const int is = 21;
	const int ir = 30;
	const double base = 1.0E9;

	bool deviateAvailable;
	float storedDeviate; //deviate from previous calculation
	int jrand;
	int* istack;
	bool init;

public:

	//Constructor and Destructor
	fastRandom();
	~fastRandom();

	double g250(int seed, double mu = 0.0, double sigma = 1.0);
	double psdrand(int iseed);
};
}

#endif // CELL_H

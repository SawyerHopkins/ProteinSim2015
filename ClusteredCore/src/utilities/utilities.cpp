/*The MIT License (MIT)

 Copyright (c) [2015] [Sawyer Hopkins]

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.*/

#include "utilities.h"

namespace PSim {
double util::safeMod(double val, double base) {
	//if the particle is outside the lower bounds.
	if (val < 0) {
		return (val + base);
	}
	//if the particle is outside the upper bounds.
	else if (val >= base) {
		return (val - base);
	}
	//No problems return value.
	else {
		return val;
	}
}

double util::safeMod0PBC(double dx, double val0, double base) {
	return (dx < 0) ? val0 - base : val0 + base;
}

double util::safeMod0(double val0, double val, double base) {
	//The difference between the two values.
	double dx = val - val0;
	//If the values are further apart than half the system, use PBC.
	return (fabs(dx) > (base / 2)) ? safeMod0PBC(dx, val0, base) : val0;
}

void util::loadBar(double x0, int n, int w) {
	/**************************************************************************************//**
	 *----------------------------------------SOURCE FROM---------------------------------------
	 *------------------------------------------------------------------------------------------
	 *---https://www.ross.click/2011/02/creating-a-progress-bar-in-c-or-any-other-console-app---
	 *------------------------------------------------------------------------------------------
	 ******************************************************************************************/
	//if ( (x != n) && (x % (n/100+1) != 0) ) return;
	int x = (int) x0;

	//Choose when to update console.
	if (x != n)
		return;

	double ratio = x / (double) n;
	int c = ratio * w;

	cout.precision(4);

	cout << setw(3) << (int) (ratio * 100) << "% [";
	for (int x = 0; x < c; x++)
		cout << "=";
	for (int x = c; x < w; x++)
		cout << " ";
	cout << "] - " << x0 << "\r" << flush;
}

double util::pbcDist(double X, double Y, double Z, double X1, double Y1,
		double Z1, double L) {

	double dx = fabs(X - X1);
	double dy = fabs(Y - Y1);
	double dz = fabs(Z - Z1);
	double half = L / 2.0;

	//Check X direction.
	if (dx > half) {
		dx -= L;
	}

	//Check Y direction.
	if (dy > half) {
		dy -= L;
	}

	//Check Z direction.
	if (dz > half) {
		dz -= L;
	}
	//Pythag for the distance.
	return (dx * dx) + (dy * dy) + (dz * dz);

}

void util::unitVectorSimple(double dX, double dY, double dZ, double r,
		double (&acc)[3]) {
	//Normalize by distance.
	acc[0] = dX / r;
	acc[1] = dY / r;
	acc[2] = dZ / r;
}

void util::unitVectorAdv(double X, double Y, double Z, double X1, double Y1,
		double Z1, double (&acc)[3], double r, int L) {
	double dx, dy, dz, oneOver;

	dx = X1 - X;
	dy = Y1 - Y;
	dz = Z1 - Z;
	oneOver = 1.0 / r;

	//Check X PBC.
	if (fabs(dx) > L / 2) {
		(dx < 0) ? dx += L : dx -= L;
	}

	//Check Y PBC.
	if (fabs(dy) > L / 2) {
		(dy < 0) ? dy += L : dy -= L;
	}

	//Check Z PBC.
	if (fabs(dz) > L / 2) {
		(dz < 0) ? dz += L : dz -= L;
	}

	//Normalize by distance.
	acc[0] = dx * oneOver;
	acc[1] = dy * oneOver;
	acc[2] = dz * oneOver;
}

double util::powBinaryDecomp(double base, int exp) {
	double answer = 1;
	while (exp) {
		if (exp & 1) {
			answer *= base;
		}
		exp >>= 1;
		base *= base;
	}
	return answer;
}

void util::setTerminalColour(Colour c) {
	switch (c) {
	case Black:
		std::cout << __BLACK;
		break;
	case Red:
		std::cout << __RED;
		break;
	case Green:
		std::cout << __GREEN;
		break;
	case Brown:
		std::cout << __BROWN;
		break;
	case Blue:
		std::cout << __BLUE;
		break;
	case Magenta:
		std::cout << __MAGENTA;
		break;
	case Cyan:
		std::cout << __CYAN;
		break;
	case Grey:
		std::cout << __GREY;
		break;
	case Normal:
		std::cout << __NORMAL;
		break;
	}
}

void util::writeTerminal(std::string text, PSim::Colour c = Normal) {
	//Change colour, write text, reset colour.
	setTerminalColour(c);
	std::cout << text;
	setTerminalColour(Normal);
}

void util::clearLine(int i, int numLines) {
	//up one line
	if (i < (numLines - 1)) {
		std::cout << "\033[A";
	}
}

void util::clearLines(int numLines) {
	if (numLines > 0) {
		for (int i = 0; i < (numLines); i++) {
			//beginning of line.
			std::cout << "\r";
			//clear line.
			std::cout << "\033[K";
			//up one line
			clearLine(i, numLines);
		}
	}
}

//"Polar" version without trigonometric calls, 1 at a time
double util::g250(int seed, double mu, double sigma) {
	static bool deviateAvailable = false; //flag
	static float storedDeviate; //deviate from previous calculation
	double polar, rsquared, var1, var2;

	//If no deviate has been stored, the polar Box-Muller transformation is
	//performed, producing two independent normally-distributed random
	//deviates. One is stored for the next round, and one is returned.
	if (!deviateAvailable) {
		//choose pairs of uniformly distributed deviates, discarding those
		//that don't fall within the unit circle
		do {
			var1 = 2.0 * (double(psdrand(seed))/*double(RAND_MAX)*/) - 1.0;
			var2 = 2.0 * (double(psdrand(seed))/*double(RAND_MAX)*/) - 1.0;
			rsquared = var1 * var1 + var2 * var2;
		} while (rsquared >= 1.0 || rsquared == 0.0);

		//calculate polar tranformation for each deviate
		polar = sqrt(-2.0 * log(rsquared) / rsquared);

		//store first deviate and set flag
		storedDeviate = var1 * polar;
		deviateAvailable = true;

		//return second deviate
		return var2 * polar * sigma + mu;
	}

	//If a deviate is available from a previous call to this function, it is
	//returned, and the flag is set to false.
	else {
		deviateAvailable = false;
		return storedDeviate * sigma + mu;
	}
}

//random number generator
double util::psdrand(int iseed) {
	int i, j, k, inx;
	double ran_num;
	static const int ndim = 55, m10 = 1000000000, is = 21, ir = 30;
	static const double base = 1.0E9;
	static int jrand, istack[58];
	static bool init = false;

	if ((!init) || (iseed < 0)) {
		iseed = abs(iseed);
		istack[ndim] = iseed;
		j = iseed;
		k = 1;

		for (i = 1; i <= (ndim - 1); ++i) {
			inx = i * is - int((double) (i * is) / (double) (ndim)) * ndim;
			istack[inx] = k;
			k = j - k;
			if (k < 0) {
				k += m10;
			}
			j = istack[inx];
		}

		for (j = 1; j <= 3; ++j) {
			for (i = 1; i <= ndim; ++i) {
				inx = i + ir - int((double) (i + ir) / (double) (ndim)) * ndim;
				istack[i] -= istack[inx + 1];
				if (istack[i] < 0) {
					istack[i] += m10;
				}
			}
		}
		jrand = 0;
		init = true;
	}

	jrand += 1;

	if (jrand > ndim) {
		for (i = 1; i <= ndim; ++i) {
			inx = i + ir - ((int) ((double) (i + ir) / (double) (ndim))) * ndim;
			istack[i] -= istack[inx + 1];
			if (istack[i] < 0) {
				istack[i] += m10;
			}
		}
		jrand = 1;
	}

	ran_num = ((double) istack[jrand]) / base;

	return ran_num;
}

std::string util::tryPop(std::queue<std::string>* queue) {
	if (queue->size() > 0) {
		std::string frontMan = queue->front();
		queue->pop();
		return frontMan;
	}
	return NULL;
}
}


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

#include "fastRandom.h"
#include "utilities.h"

namespace PSim {

fastRandom::fastRandom() {
	deviateAvailable = false;
	storedDeviate = 0;
	jrand = 0;
	istack = new int[58];
	init = false;
}

fastRandom::~fastRandom() {
}

//random number generator
double fastRandom::psdrand(int iseed) {
	int i, j, k, inx;
	double ran_num;

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

//"Polar" version without trigonometric calls, 1 at a time
double fastRandom::g250(int seed, double mu, double sigma) {
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
}

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

#include "error.h"

namespace debugging
{
	__device__ __host__
	void error::throwInitializationError()
	{
		#if __CUDA_ARCH__ > 0
		printf("Could not create initial system.\nTry decreasing particle density");
		#else
		std::cout << "Could create initial system.\n";
		std::cout << "Try decreasing particle density\n.";
		exit(7701);
		#endif
	}

	__device__ __host__
	void error::throwCellBoundsError(int cx, int cy, int cz)
	{
		#if __CUDA_ARCH__ > 0
		printf("Unable to find: cells[%d,%d,%d]\n",cx,cy,cz);
		#else
		std::cout << "Unable to find: cells[" << cx << "][" << cy << "][" << cz << "]";
		exit(7702);
		#endif
	}

	__device__ __host__
	void error::throwParticleBoundsError(int x, int y, int z)
	{
		#if __CUDA_ARCH__ > 0
		printf("Particle out of bounds: %f, %f, %f\n", x, y, z);
		#else
		std::cout << "\nParticle out of bounds.\n";
		std::cout << x << "," << y << "," << z << "\n";
		exit(7703);
		#endif
	}

	__device__ __host__
	void error::throwParticleOverlapError(int nameI, int nameJ, float r)
	{
		#if __CUDA_ARCH__ > 0
		printf("Significant particle overlap. Consider time-step reduction: %f, %d, %d\n", r, nameI, nameJ);
		#else
		std::cout << "\nSignificant particle overlap. Consider time-step reduction.\n";
		std::cout << "\nR: " << r;
		std::cout << "\n" << "I-Name:" << nameI;
		std::cout << "\n" << "J-Name:" << nameJ;
		exit(7704);
		#endif
	}

	__device__ __host__
	void error::throwInfiniteForce()
	{
		#if __CUDA_ARCH__ > 0
		printf("Bad news bears. Numerically unstable system.\n");
		#else
		std::cout << "\nBad news bears; Numerically unstable system.";
		std::cout << "\nAttempt reduction of concentration or decreased time step.";
		exit(7705);
		#endif
	}

	__device__ __host__
	void error::throwInputError()
	{
		#if __CUDA_ARCH__ > 0
		printf("Invalid input file.\n");
		#else
		std::cout << "\n" << "Invalid input file." << "\n";
		exit(7706);
		#endif
	}
}


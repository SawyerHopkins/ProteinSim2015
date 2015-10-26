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

	void error::throwInitializationError()
	{
		std::cout << "Could create initial system.\n";
		std::cout << "Try decreasing particle density\n.";
		exit(7701);
	}

	void error::throwCellBoundsError(int cx, int cy, int cz)
	{
		std::cout << "Unable to find: cells[" << cx << "][" << cy << "][" << cz << "]";
		exit(7702);
	}

	void error::throwParticleBoundsError(float x, float y, float z, int name)
	{
		std::cout << "\nParticle out of bounds.\n";
		std::cout << "Particle: " << name << " : " << x << "," << y << "," << z << "\n";
		exit(7703);
	}

	void error::throwParticleOverlapError(int nameI, int nameJ, double r)
	{
		std::cout << "\nSignificant particle overlap. Consider time-step reduction.\n";
		std::cout << "\nR: " << r;
		std::cout << "\n" << "I-Name:" << nameI;
		std::cout << "\n" << "J-Name:" << nameJ;
		exit(7704);
	}

	void error::throwInfiniteForce()
	{
		std::cout << "\nBad news bears; Numerically unstable system.";
		std::cout << "\nAttempt reduction of concentration or decreased time step.";
		exit(7705);
	}

	void error::throwInputError()
	{
		std::cout << "\n" << "Invalid input file." << "\n";
		exit(7706);
	}

}


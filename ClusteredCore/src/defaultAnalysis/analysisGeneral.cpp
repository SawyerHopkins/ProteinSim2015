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

#include "analysisManager.h"

namespace PSim {
double analysisManager::getTemperature(particle** particles, int nParticles) {
	double vtot = 0;

	for (int i = 0; i < nParticles; i++) {
		//Add the total velocity squares.
		double vx = particles[i]->getVX();
		double vy = particles[i]->getVY();
		double vz = particles[i]->getVZ();
		vtot += (vx * vx);
		vtot += (vy * vy);
		vtot += (vz * vz);
	}

	//Important physics
	vtot = vtot / 3.0;
	vtot = vtot / nParticles;
	//Average
	return vtot;
}

double analysisManager::meanDisplacement(particle** particles, int nParticles) {
	double disp = 0;

	for (int i = 0; i < nParticles; i++) {
		double dx = particles[i]->getX() - particles[i]->getX0();
		double dy = particles[i]->getY() - particles[i]->getY0();
		double dz = particles[i]->getZ() - particles[i]->getZ0();
		disp += (dx * dx + dy * dy + dz * dz);
	}
	return (disp / double(nParticles));
}
}

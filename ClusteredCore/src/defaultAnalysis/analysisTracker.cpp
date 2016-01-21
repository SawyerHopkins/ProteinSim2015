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
void analysisManager::initDisplacementTracker(particle** particles, int nParticles) {
	for (int i = 0; i < nParticles; i++)
	{
		xStart[i] = particles[i]->getX();
		yStart[i] = particles[i]->getY();
		zStart[i] = particles[i]->getZ();
		xPBC[i] = 0;
		yPBC[i] = 0;
		zPBC[i] = 0;
	}
}

void analysisManager::updateParticlePBC(int i, particle** particles) {
	//Check boundary condition crossings..
	if (particles[i]->getX0() < 0) {
		xPBC[i] += 1;
	} else if (particles[i]->getX0() > boxSize) {
		xPBC[i] -= 1;
	}

	if (particles[i]->getY0() < 0) {
		yPBC[i] += 1;
	} else if (particles[i]->getY0() > boxSize) {
		yPBC[i] -= 1;
	}

	if (particles[i]->getZ0() < 0) {
		zPBC[i] += 1;
	} else if (particles[i]->getZ0() > boxSize) {
		zPBC[i] -= 1;
	}
}

void analysisManager::updateTracker(particle** particles, int nParticles) {
	for (int i = 0; i < nParticles; i++)
	{
		updateParticlePBC(i, particles);
	}
}

double analysisManager::trackedDisplacement(particle** particles, int nParticles) {
	float dr = 0.0;
	for (int i = 0; i < nParticles; i++)
	{
		//Check boundary condition crossings..
		updateParticlePBC(i, particles);

		float dx = particles[i]->getX() - xStart[i];
		dx += boxSize * xPBC[i];
		dr += (dx*dx);
		float dy = particles[i]->getY() - yStart[i];
		dy += boxSize * yPBC[i];
		dr += (dy*dy);
		float dz = particles[i]->getZ() - zStart[i];
		dz += boxSize * zPBC[i];
		dr += (dz*dz);
	}
	dr /= nParticles;
	return dr;
}
}

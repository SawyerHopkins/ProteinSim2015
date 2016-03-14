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

#include "system.h"

using namespace std;

namespace PSim {

/********************************************//**
 *---------------PARTICLE HANDLING----------------
 ************************************************/

void system::hashParticles() {
	int cellScaleSq = state.cellScale * state.cellScale;
#pragma omp parallel for
	for (int i = 0; i < state.nParticles; i++) {
		type3<int> itemCell;

		itemCell.x = floor(particles[i]->getX() / state.cellSize);
		itemCell.y = floor(particles[i]->getY() / state.cellSize);
		itemCell.z = floor(particles[i]->getZ() / state.cellSize);

		int hash = itemCell.x + (state.cellScale * itemCell.y) + (cellScaleSq * itemCell.z);

		get<0>(particleHashIndex[i]) = hash;
		get<1>(particleHashIndex[i]) = i;
	}
}

void system::sortParticles() {
	__gnu_parallel::sort(particleHashIndex.begin(), particleHashIndex.end(), util::sortParticleTuple);
}

void system::reorderParticles() {
#pragma omp parallel for
	for (int i = 0; i < state.nParticles; i++) {
		// Set Cell Data.
		int currentHash = get<0>(particleHashIndex[i]);

		if (i == 0) {
			get<0>(cellStartEnd[currentHash]) = 0;
		}

		if (i > 0) {
			int prevHash = get<0>(particleHashIndex[i-1]);

			if (prevHash != currentHash) {
				get<0>(cellStartEnd[currentHash]) = i;
				get<1>(cellStartEnd[prevHash]) = i;
			}
		}

		if (i == state.nParticles - 1) {
			get<1>(cellStartEnd[currentHash]) = state.nParticles;
		}

		// Copy Particle Data.
		int index = get<1>(particleHashIndex[i]);
		int offset = 4*i;
		sortedParticles[offset] = particles[index]->getX();
		sortedParticles[offset+1] = particles[index]->getY();
		sortedParticles[offset+2] = particles[index]->getZ();
		sortedParticles[offset+3] = particles[index]->getRadius();
	}
}

void system::pushParticleForce() {
#pragma omp parallel for
	for (int i =0; i < state.nParticles; i++) {
		particles[i]->setForce(&(particleForce[3*i]));
	}
}

}

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

void system::iterateParticleInteractions(int index, int hash) {
	int start = get<0>(cellStartEnd[hash]);

	type3<double> cellForce = type3<double>();

	if (start != 0xffffffff) {
		int end = get<1>(cellStartEnd[hash]);
		for (int i=start; i<end; i++) {
			if (i != index) {
				int indexOffset = 4*index;
				int iOffset = 4*i;

				double rSquared = PSim::util::pbcDist(sortedParticles[indexOffset], sortedParticles[indexOffset+1], sortedParticles[indexOffset+2],
																	sortedParticles[iOffset], sortedParticles[iOffset+1], sortedParticles[iOffset+2],
																	state.boxSize);


				double r = sqrt(rSquared);
				//If the particles are in the potential well.
				if (r < (1.2))
				{
					int realIndex = get<1>(particleHashIndex[index]);
					int realI = get<1>(particleHashIndex[i]);
					particles[realIndex]->addInteraction(particles[realI]);
				}
			}
		}
	}
}

void system::updateInteractions() {
	using namespace std;
#pragma omp parallel for
	for (int index=0; index < state.nParticles; index++) {
		int hash = 0;
		int indexOffset = index*4;
		type3<int> cell = type3<int>();
		type3<int> cRef = type3<int>();
		int scale = state.cellScale;
		int cellScaleSq = scale*scale;

		cell.x = floor(sortedParticles[indexOffset] / state.cellSize);
		cell.y = floor(sortedParticles[indexOffset+1] / state.cellSize);
		cell.z = floor(sortedParticles[indexOffset+2] / state.cellSize);

		for (int x=-1; x<=1; x++) {
			for (int y=-1; y<=1; y++) {
				for (int z=-1; z<=1; z++) {
					cRef.x = cell.x + x;
					cRef.y = cell.y + y;
					cRef.z = cell.z + z;

					cRef.x = cRef.x % scale;
					cRef.y = cRef.y % scale;
					cRef.z = cRef.z % scale;

					cRef.x = (cRef.x < 0) ? cRef.x + scale : cRef.x;
					cRef.y = (cRef.y < 0) ? cRef.y + scale : cRef.y;
					cRef.z = (cRef.z < 0) ? cRef.z + scale : cRef.z;

					hash = cRef.x + (scale * cRef.y) + (cellScaleSq * cRef.z);

					iterateParticleInteractions(index, hash);
				}
			}
		}
	}
}

}

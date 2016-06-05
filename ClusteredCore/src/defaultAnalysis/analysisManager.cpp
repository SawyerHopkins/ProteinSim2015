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

analysisManager::analysisManager(string tName, systemState* state) {
	trialName = tName;
	int nParticles = state->nParticles;
	xStart = new float [nParticles];
	yStart = new float [nParticles];
	zStart = new float [nParticles];
	xPBC = new int[nParticles];
	yPBC = new int[nParticles];
	zPBC = new int[nParticles];
	counter = 0;
	boxSize = state->boxSize;

	for (int i = 0; i < nParticles; i++)
	{
		xStart[i] = 0;
		yStart[i] = 0;
		zStart[i] = 0;
		xPBC[i] = 0;
		yPBC[i] = 0;
		zPBC[i] = 0;
	}
}

void analysisManager::writeInitialState(particle** particles, systemState* state) {
	int nParticles = state->nParticles;
	initDisplacementTracker(particles, nParticles);
	writeInitTemp(particles, nParticles);
	writeSystem(particles, nParticles, trialName + "/initialState");
	std::string movName = trialName + "/movie/system-initial";
	writeSystemXYZ(particles, nParticles, true, 0, movName);
}

void analysisManager::writeRunTimeState(particle** particles, systemState* state) {
	//Output a snapshot every second.
	if ((counter % state->outputFreq) == 0) {
		if (state->currentTime > 0) {
			PSim::util::clearLines(-1);
		}
		writeSystemState(particles, state->nParticles, state->currentTime);
	} else {
		updateTracker(particles, state->nParticles);
	}
	counter++;
}

void analysisManager::writeFinalState(particle** particles, systemState* state) {
	writeSystem(particles, state->nParticles, trialName + "/finalState");
}

}




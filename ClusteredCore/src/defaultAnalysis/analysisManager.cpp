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

analysisManager::analysisManager(string tName, int nParticles, int size) {
	trialName = tName;
	xStart = new float [nParticles];
	yStart = new float [nParticles];
	zStart = new float [nParticles];
	xPBC = new float[nParticles];
	yPBC = new float[nParticles];
	zPBC = new float[nParticles];
	counter = 0;
	boxSize = size;

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

void analysisManager::writeInitialState(particle** particles, int nParticles) {
	initDisplacementTracker(particles, nParticles);
	writeInitTemp(particles, nParticles);
	writeSystem(particles, nParticles, trialName + "/initialState");
}

void analysisManager::writeRunTimeState(particle** particles, int nParticles, int outXYZ, int outputFreq, double currentTime) {
	//Output a snapshot every second.
	if ((counter % outputFreq) == 0) {
		if (currentTime > 0) {
			PSim::util::clearLines(14);
		}
		writeSystemState(particles, nParticles, outXYZ, currentTime);
	} else {
		updateTracker(particles, nParticles);
	}
	counter++;
}

void analysisManager::writeFinalState(particle** particles, int nParticles) {
	writeSystem(particles, nParticles, trialName + "/finalState");
}

}




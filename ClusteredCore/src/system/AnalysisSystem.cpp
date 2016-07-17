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

#include "analysisSystem.h"

namespace PSim {

AnalysisSystem::AnalysisSystem(config* cfg,
		std::string sysState,
		std::string timeStamp, IAnalysisManager* analysisInterface) : system(cfg, NULL, NULL, sysState + "/-analysis-" + timeStamp)
{
	// Read in each particle.
	int nParts = particlesInFile(sysState, timeStamp);
	// Overwrite the particle system with the old one.
	int count = readParticles(sysState, timeStamp);

	// Maybe this should throw an error for count != nParts?
	chatterBox.consoleMessage("Found " + tos(count) + " / " + tos(nParts) + " particles", 1);

	analysis = (analysisInterface == NULL) ? defaultAnalysisInterface() : analysisInterface;
	createRewindDir();

	// We need to rebuild the interactions table.
	hashParticles();
	sortParticles();
	clearCells();
	reorderParticles();
	updateInteractions();
}

}

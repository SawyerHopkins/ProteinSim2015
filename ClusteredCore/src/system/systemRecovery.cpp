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

namespace PSim {

/********************************************//**
 *-----------------SYSTEM RECOVERY----------------
 ***********************************************/

int system::particlesInFile(std::string sysState, std::string timeStamp) {
	std::string fullPath = sysState + "/snapshots/time-" + timeStamp
			+ "/recovery.txt";
	ifstream state;
	state.open(fullPath, ios_base::in);

	int count = 0;
	for (std::string line; std::getline(state, line);) {
		count++;
	}
	return count;
}

int system::readParticles(std::string sysState,
		std::string timeStamp) {
	std::string fullPath = sysState + "/snapshots/time-" + timeStamp
			+ "/recovery.txt";
	chatterBox.consoleMessage("Reading: " + fullPath, 1);
	ifstream streamState;
	streamState.open(fullPath, ios_base::in);
	// Read in each particle.
	int count = 0;
	for (std::string line; std::getline(streamState, line);) {
		istringstream data(line);

		type3<double>* pos = new type3<double>();
		type3<double>* pos0 = new type3<double>();
		type3<double>* frc = new type3<double>();
		type3<double>* frc0 = new type3<double>();

		float m, r;

		data >> pos->x >> pos->y >> pos->z;
		data >> pos0->x >> pos0->y >> pos0->z;
		data >> frc->x >> frc->y >> frc->z;
		data >> frc0->x >> frc0->y >> frc0->z;
		data >> m >> r;

		// Set each particle to the oldest know position and advance to the newest known position.
		particles[count] = new particle(count);
		particles[count]->setPos(pos0, state.boxSize);
		particles[count]->updateForce(frc0, NULL, false);
		particles[count]->nextIter();
		particles[count]->setPos(pos, state.boxSize);
		particles[count]->updateForce(frc, NULL, false);
		particles[count]->setMass(m);
		particles[count]->setRadius(r);

		count++;
	}
	return count;
}

void system::readSettings(config* cfg) {
	cfg->showOutput();
	state.concentration = cfg->getParam<double>("Concentration", 0);
	state.cellSize = cfg->getParam<int>("cellSize", 0);
	state.cellScale = cfg->getParam<int>("cellScale", 0);
	state.temp = cfg->getParam<double>("temp", 0);
	state.currentTime = 0;
	state.dTime = cfg->getParam<double>("dTime", 0);
	state.outputFreq = cfg->getParam<int>("outputFreq", 0);
	cycleHour = cfg->getParam<double>("cycleHour", 0);
	state.seed = cfg->getParam<int>("seed", 0);
	state.boxSize = cfg->getParam<int>("boxSize", 0);
	cfg->hideOutput();
}

void system::createRewindDir() {
	//Check that the provided directory exists.
	bool validDir = checkDir(trialName);
	if (validDir == true) {
		PSim::util::writeTerminal(
				"\nTrial name already exists. Overwrite (y,n): ",
				PSim::Colour::Magenta);

		//Check user input
		std::string cont;
		std::cin >> cont;

		if (cont != "Y" && cont != "y") {
			trialName = dirPrompt();
		}
	} else {
		//Attempt to make the directory.
		mkdir(trialName.c_str(), 0777);

		//Check that we were able to make the desired directory.
		validDir = checkDir(trialName);
		if (validDir == false) {
			trialName = dirPrompt();
		}
	}
}

}

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

void system::createInteractionsTable() {
	/*
	double cutOffSquared = 1.1 * 1.1;
	int boxSize = state.boxSize;
	for (int i = 0; i < state.nParticles; i++) {
		PSim::particle* base = particles[i];
		PSim::PeriodicGrid* itemCell =
				cells[base->getCX()][base->getCY()][base->getCZ()];
		for (auto currentCell = itemCell->getFirstNeighbor();
				currentCell != itemCell->getLastNeighbor(); ++currentCell) {
			for (auto member = (*currentCell)->getBegin();
					member != (*currentCell)->getEnd(); ++member) {
				if (member->second->getName() != base->getName()) {
					//Distance between the two particles.
					double rSquared = PSim::util::pbcDist(base->getX(),
							base->getY(), base->getZ(), member->second->getX(),
							member->second->getY(), member->second->getZ(),
							boxSize);

					//If the particles are in the potential well.
					if (rSquared < cutOffSquared) {
						base->addInteraction(member->second);
					}
				}
			}
		}
	}
	*/
}

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

int system::readParticles(system* oldSys, std::string sysState,
		std::string timeStamp, int bsize) {
	std::string fullPath = sysState + "/snapshots/time-" + timeStamp
			+ "/recovery.txt";
	chatterBox.consoleMessage("Reading: " + fullPath, 1);
	ifstream state;
	state.open(fullPath, ios_base::in);
	// Read in each particle.
	int count = 0;
	for (std::string line; std::getline(state, line);) {
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
		oldSys->particles[count] = new particle(count);
		oldSys->particles[count]->setPos(pos0, bsize);
		oldSys->particles[count]->updateForce(frc0, NULL, false);
		oldSys->particles[count]->nextIter();
		oldSys->particles[count]->setPos(pos, bsize);
		oldSys->particles[count]->updateForce(frc, NULL, false);
		oldSys->particles[count]->setMass(m);
		oldSys->particles[count]->setRadius(r);

		count++;
	}
	return count;
}

void system::readSettings(system* oldSys, config* cfg) {
	cfg->showOutput();
	oldSys->state.concentration = cfg->getParam<double>("Concentration", 0);
	oldSys->state.cellSize = cfg->getParam<int>("cellSize", 0);
	oldSys->state.cellScale = cfg->getParam<int>("cellScale", 0);
	oldSys->state.temp = cfg->getParam<double>("temp", 0);
	oldSys->state.currentTime = 0;
	oldSys->state.dTime = cfg->getParam<double>("dTime", 0);
	oldSys->state.outputFreq = cfg->getParam<int>("outputFreq", 0);
	oldSys->cycleHour = cfg->getParam<double>("cycleHour", 0);
	oldSys->state.seed = cfg->getParam<int>("seed", 0);
	oldSys->state.boxSize = cfg->getParam<int>("boxSize", 0);
	cfg->hideOutput();
}

void system::createRewindDir(system* oldSys) {
	//Check that the provided directory exists.
	bool validDir = checkDir(oldSys->trialName);
	if (validDir == true) {
		PSim::util::writeTerminal(
				"\nTrial name already exists. Overwrite (y,n): ",
				PSim::Colour::Magenta);

		//Check user input
		std::string cont;
		std::cin >> cont;

		if (cont != "Y" && cont != "y") {
			oldSys->trialName = dirPrompt();
		}
	} else {
		//Attempt to make the directory.
		mkdir(oldSys->trialName.c_str(), 0777);

		//Check that we were able to make the desired directory.
		validDir = checkDir(oldSys->trialName);
		if (validDir == false) {
			oldSys->trialName = dirPrompt();
		}
	}
}

system* system::loadFromFile(config* cfg, std::string sysState,
		std::string timeStamp, PSim::IIntegrator* sysInt,
		PSim::defaultForceManager* sysFcs) {
	PSim::util::writeTerminal("\n\nLoading recovery state...\n",
			PSim::Colour::Green);

	system* oldSys = new system();

	int bsize = cfg->getParam<int>("boxSize", 0);
	int nParts = cfg->getParam<int>("nParticles", 0);

	// Load in the particles.
	oldSys->particles = new particle*[nParts];
	int count = readParticles(oldSys, sysState, timeStamp, bsize);

	// Maybe this should throw an error for count != nParts?
	chatterBox.consoleMessage("Found " + tos(count) + " / " + tos(nParts) + " particles", 1);

	// Pulls in the old system configuration. Don't recalculate it incase the settings.cfg gets changed incorrectly.
	readSettings(oldSys, cfg);
	oldSys->trialName = sysState + "/-rewind-" + timeStamp;
	oldSys->analysis = oldSys->defaultAnalysisInterface(count, bsize);
	oldSys->state.nParticles = count;
	oldSys->integrator = sysInt;
	oldSys->sysForces = sysFcs;

	//oldSys->initCells(oldSys->state.cellScale);
	createRewindDir(oldSys);
	oldSys->writeSystemInit();
	return oldSys;
}

system* system::loadAnalysis(config* cfg, std::string sysState,
		std::string timeStamp, IAnalysisManager* analysisInterface) {
	system* oldSys = new system();

	// Read in each particle.
	int nParts = particlesInFile(sysState, timeStamp);
	oldSys->particles = new particle*[nParts];

	int bsize = cfg->getParam<int>("boxSize", 0);
	int count = readParticles(oldSys, sysState, timeStamp, bsize);
	chatterBox.consoleMessage("Found " + tos(count) + " / " + tos(nParts) + " particles", 1);


	readSettings(oldSys, cfg);
	oldSys->state.nParticles = count;
	oldSys->trialName = sysState + "/-analysis-" + timeStamp;
	oldSys->analysis = (analysisInterface == NULL) ? oldSys->defaultAnalysisInterface(count, bsize) : analysisInterface;

	//oldSys->initCells(oldSys->state.cellScale);
	createRewindDir(oldSys);
	oldSys->createInteractionsTable();
	return oldSys;
}
}

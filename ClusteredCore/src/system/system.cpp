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

void system::setSystemConstants(config* cfg,
		PSim::IIntegrator* sysInt, PSim::defaultForceManager* sysFcs) {

	state = systemState();

	//Set time information
	state.currentTime = 0;
	state.dTime = cfg->getParam<double>("timeStep", 0.001);
	//Set the random number generator seed.
	state.seed = cfg->getParam<int>("seed", 90210);
	//Sets the system temperature.
	state.temp = cfg->getParam<double>("kT", 1.0);
	//Set the number of particles.
	state.nParticles = cfg->getParam<int>("nParticles", 1000);
	//How often to output snapshots.
	state.outputFreq = cfg->getParam<int>("outputFreq", int(1.0 / state.dTime));
	//Set initial seed size;
	seedSize = cfg->getParam<int>("seedSize", 0);
	//Set the integration method.
	integrator = sysInt;
	//Set the internal forces.
	sysForces = sysFcs;
	//Set the concentration.
	double conc = cfg->getParam<double>("conc", 0.01);
	//Set the scale.
	int scale = 0;
	scale = cfg->getParam<int>("scale", 4);
	//Set the radius.
	double r = cfg->getParam<double>("radius", 0.5);
	//Create a box based on desired concentration.
	double vP = state.nParticles * (4.0 / 3.0) * atan(1.0) * 4.0 * r * r * r;
	state.boxSize = (int) (cbrt(vP / conc));
	//Calculates the number of cells needed.
	state.cellSize = state.boxSize / scale;
	state.boxSize = state.cellSize * scale;
	state.cellScale = scale;
	//Sets the actual concentration.
	state.concentration = vP / pow(state.boxSize, 3.0);

	analysis = new PSim::analysisManager(trialName, &state);
}

system::system(config* cfg, PSim::IIntegrator* sysInt,
		PSim::defaultForceManager* sysFcs) {

	//Sets the trial name
	trialName = cfg->getParam<std::string>("trialName", "");

	if (trialName == "") {
		dirPrompt();
	} else {
		//Check that the provided directory exists.
		verifyPath();
	}

	//Set time information
	setSystemConstants(cfg, sysInt, sysFcs);
	chatterBox.consoleMessage("System concentration: " + tos(state.concentration), 3);

	//Set the radius.
	double r = cfg->getParam<double>("radius", 0.5);
	//Set the mass.
	double m = cfg->getParam<double>("mass", 1.0);
	//Create particles.
	initParticles(r, m);
	//Create cells.
	int numCells = pow(state.cellScale, 3.0);
	particleHashIndex = vector<tuple<int,int>>(state.nParticles, tuple<int,int>());
	cellStartEnd = vector<tuple<int,int>>(numCells, tuple<int,int>(0xffffffff, 0xffffffff));
	sortedParticles = new double[4*state.nParticles];
	particleForce  = new double[3*state.nParticles];
	hashParticles();
	sortParticles();
	reorderParticles();
	chatterBox.consoleMessage("Created: " + tos(numCells) + " cells from scale: " + tos(state.cellScale));
	writeSystemInit();
}

void system::writeSystemInit() {
	using namespace std;

	ofstream myFile;
	myFile.open(trialName + "/sysConfig.cfg");

	//Writes the system configuration.
#ifdef WITHPOST
	myFile << "Mode = WITHPOST\n";
#endif
	myFile << "trialName = " << trialName << "\n";
	myFile << "nParticles = " << state.nParticles << "\n";
	myFile << "Concentration = " << state.concentration << "\n";
	myFile << "boxSize = " << state.boxSize << "\n";
	myFile << "cellSize = " << state.cellSize << "\n";
	myFile << "cellScale = " << state.cellScale << "\n";
	myFile << "temp = " << state.temp << "\n";
	myFile << "dTime = " << state.dTime << "\n";
	myFile << "outputFreq = " << state.outputFreq << "\n";
	myFile << "cycleHour = " << cycleHour << "\n";
	myFile << "seed = " << state.seed;

	//Close the stream.
	myFile.close();

	analysis->writeInitialState(particles, &state);

	ifstream inCfg("settings.cfg", ios::binary);
	ofstream outCfg(trialName + "/settings.cfg", ios::binary);

	outCfg << inCfg.rdbuf();

}

system::~system() {
	//Deletes the particles
	for (int i = 0; i < state.nParticles; i++) {
		delete particles[i];
	}
	delete[] particles;

	//Delete the constants.
	delete &state;
	delete[] integrator;
	delete[] sysForces;
}

void system::estimateCompletion(PSim::timer* tmr) {
	int cycleCount = (state.currentTime/state.dTime);
	if ((cycleCount % state.outputFreq) == 0 && cycleCount > 0) {
		tmr->stop();
		double timePerCycle = tmr->getElapsedSeconds() / double(state.outputFreq);
		std::setprecision(4);
		chatterBox.consoleMessage("Average Cycle Time: " + tos(timePerCycle));
		double totalTime = (cycleHour * timePerCycle);
		double finishedTime = ((state.currentTime / state.dTime) / 3600) * timePerCycle;
		double dif = totalTime - finishedTime;
		chatterBox.consoleMessage("Time until completion: " + tos(dif) + " hours.");
		tmr->start();
	}
}

void system::run(double endTime) {
	state.endTime = endTime;
	cycleHour = (state.endTime / state.dTime) / 3600.0;
	//Create the snapshot name.
	std::string snap = trialName + "/snapshots";
	mkdir(snap.c_str(), 0777);

	//Create the movie folder
	std::string mov = trialName + "/movie";
	mkdir(mov.c_str(), 0777);

	//Diagnostics timer.
	PSim::timer* tmr = new PSim::timer();
	tmr->start();

	chatterBox.resetChatterCount();
	//Run system until end time.
	while (state.currentTime < state.endTime) {
		//Get the forces acting on the system.
		sysForces->getAcceleration(sortedParticles, particleForce, &particleHashIndex, &cellStartEnd, &state);
#ifdef WITHPOST
		sysForces->getPostRoutine(sortedParticles, particleForce, &particleHashIndex, &cellStartEnd, &state);
#endif
		// Update the particle system
		pushParticleForce();
		//Get the next system.
		integrator->nextSystem(particles, &state);
		// Rebuild the hash table
		hashParticles();
		sortParticles();
		std::fill(cellStartEnd.begin(), cellStartEnd.end(), tuple<int,int>(0xffffffff, 0xffffffff));
		reorderParticles();
		// Get new particle interactions
		updateInteractions();
		//runAnalysis;
		analysis->writeRunTimeState(particles, &state);
		estimateCompletion(tmr);
		//Update loading bar.
		PSim::util::loadBar(state.currentTime, state.endTime);
		//Increment counters.
		state.currentTime += state.dTime;
	}
}
}


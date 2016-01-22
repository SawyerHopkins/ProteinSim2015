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

void system::setSystemConstants(configReader::config* cfg,
		PSim::IIntegrator* sysInt, PSim::defaultForceManager* sysFcs) {
	//Set time information
	currentTime = 0;
	dTime = cfg->getParam<double>("timeStep", 0.001);
	//Set the random number generator seed.
	seed = cfg->getParam<int>("seed", 90210);
	//Sets the system temperature.
	temp = cfg->getParam<double>("temp", 1.0);
	//Set the number of particles.
	nParticles = cfg->getParam<int>("nParticles", 1000);
	//How often to output snapshots.
	outputFreq = cfg->getParam<int>("outputFreq", int(1.0 / dTime));
	//Option to output XYZ format for clusters
	outXYZ = cfg->getParam<int>("XYZ", 0);
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
	double vP = nParticles * (4.0 / 3.0) * atan(1.0) * 4.0 * r * r * r;
	boxSize = (int) (cbrt(vP / conc));
	//Calculates the number of cells needed.
	cellSize = boxSize / scale;
	boxSize = cellSize * scale;
	cellScale = scale;
	//Sets the actual concentration.
	concentration = vP / pow(boxSize, 3.0);

	analysis = new PSim::analysisManager(trialName, nParticles, boxSize);
}

system::system(configReader::config* cfg, PSim::IIntegrator* sysInt,
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
	std::cout << "---System concentration: " << concentration << "\n";

	//Set the radius.
	double r = cfg->getParam<double>("radius", 0.5);
	//Set the mass.
	double m = cfg->getParam<double>("mass", 1.0);
	//Create particles.
	initParticles(r, m);
	//Create cells.
	int numCells = pow(cellScale, 3.0);
	initCells(cellScale);
	std::cout << "Created: " << numCells << " cells from scale: " << cellScale
			<< "\n";

	writeSystemInit();
}

void system::writeSystemInit() {
	using namespace std;

	ofstream myFile;
	myFile.open(trialName + "/sysConfig.cfg");

	//Writes the system configuration.
	myFile << "trialName = " << trialName << "\n";
	myFile << "nParticles = " << nParticles << "\n";
	myFile << "Concentration = " << concentration << "\n";
	myFile << "boxSize = " << boxSize << "\n";
	myFile << "cellSize = " << cellSize << "\n";
	myFile << "cellScale = " << cellScale << "\n";
	myFile << "temp = " << temp << "\n";
	myFile << "dTime = " << dTime << "\n";
	myFile << "outputFreq = " << outputFreq << "\n";
	myFile << "outXYZ = " << outXYZ << "\n";
	myFile << "cycleHour = " << cycleHour << "\n";
	myFile << "seed = " << seed;

	//Close the stream.
	myFile.close();

	analysis->writeInitialState(particles, nParticles);

	ifstream inCfg("settings.cfg", ios::binary);
	ofstream outCfg(trialName + "/settings.cfg", ios::binary);

	outCfg << inCfg.rdbuf();

}

system::~system() {
	//Deletes the particles
	for (int i = 0; i < nParticles; i++) {
		delete particles[i];
	}
	delete[] particles;

	//Delete the constants.
	delete &nParticles;
	delete &concentration;
	delete &boxSize;
	delete &cellSize;
	delete &temp;
	delete &currentTime;
	delete &dTime;
	delete &seed;
	delete[] integrator;
	delete[] sysForces;
}

void system::estimateCompletion(PSim::timer* tmr) {
	int cycleCount = (currentTime/dTime);
	if ((cycleCount % outputFreq) == 0 && cycleCount > 0) {
		tmr->stop();
		double timePerCycle = tmr->getElapsedSeconds() / double(outputFreq);
		std::setprecision(4);
		std::cout << "\n" << "Average Cycle Time: " << timePerCycle
				<< " seconds.\n";
		double totalTime = (cycleHour * timePerCycle);
		double finishedTime = ((currentTime / dTime) / 3600) * timePerCycle;
		std::cout << "Time for completion: " << (totalTime - finishedTime)
				<< " hours.\n";
		tmr->start();
	}
}

void system::run(double endTime) {
	cycleHour = (endTime / dTime) / 3600.0;
	//Create the snapshot name.
	std::string snap = trialName + "/snapshots";
	mkdir(snap.c_str(), 0777);

	//Create the movie folder
	std::string mov = trialName + "/movie";
	mkdir(mov.c_str(), 0777);

	//Diagnostics timer.
	PSim::timer* tmr = new PSim::timer();
	tmr->start();

	//Run system until end time.
	while (currentTime < endTime) {
		//Get the forces acting on the system.
		//sysForces->getAcceleration(nParticles, boxSize, currentTime, cells,
		//		particles);
		//Get the next system.
		integrator->nextSystem(currentTime, dTime, nParticles, boxSize, cells,
				particles, sysForces);
		//Call cell manager.
		updateCells();

		//runAnalysis;
		analysis->writeRunTimeState(particles, nParticles, outXYZ, outputFreq, currentTime);
		estimateCompletion(tmr);
		//Update loading bar.
		PSim::util::loadBar(currentTime, endTime);

		//Increment counters.
		currentTime += dTime;
	}
}
}


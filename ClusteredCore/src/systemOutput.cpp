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
 *-----------------SYSTEM OUTPUT------------------
 ************************************************/

void system::writeSystemXYZ(std::string name) {
	//Check if XYZ output is enabled.
	if (outXYZ > 0) {
		//Create a stream to the desired file.
		std::ofstream myFile;
		myFile.open(name + ".xyz");

		//Write xyz header
		myFile << nParticles << "\n";
		myFile << "Current Time: "
				<< std::to_string(int(std::round(currentTime))) << "\n";

		//Write the particle positions in XYZ format, spoofing all particles as Hydrogen.
		for (int i = 0; i < nParticles; i++) {
			myFile << "H " << particles[i]->getX() << " "
					<< particles[i]->getY() << " " << particles[i]->getZ()
					<< "\n";
		}
	}
}

void system::writeSystem(std::string name) {
	//Create a stream to the desired file.
	std::ofstream myFile;
	myFile.open(name + ".txt");
	//Write each point in the system as a line of csv formatted as: X,Y,Z
	for (int i = 0; i < nParticles; i++) {
		myFile << particles[i]->getX() << " " << particles[i]->getY() << " "
				<< particles[i]->getZ() << " ";
		myFile << particles[i]->getX0() << " " << particles[i]->getY0() << " "
				<< particles[i]->getZ0() << " ";
		myFile << particles[i]->getFX() << " " << particles[i]->getFY() << " "
				<< particles[i]->getFZ() << " ";
		myFile << particles[i]->getFX0() << " " << particles[i]->getFY0() << " "
				<< particles[i]->getFZ0() << " ";
		myFile << particles[i]->getMass() << " " << particles[i]->getRadius();
		if (i < (nParticles - 1)) {
			myFile << "\n";
		}
	}
	//Close the stream.
	myFile.close();
}

void system::writeInitTemp() {
	double v2 = 0.0;
	//Get V^2 for each particle.
	for (int i = 0; i < nParticles; i++) {
		v2 += particles[i]->getVX() * particles[i]->getVX();
		v2 += particles[i]->getVY() * particles[i]->getVY();
		v2 += particles[i]->getVZ() * particles[i]->getVZ();
	}
	//Average v2.
	double vAvg = v2 / float(nParticles);
	double temp = (vAvg / 3.0);
	//
	std::cout << "---Temp: " << temp << " m/k" << "\n";
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

	writeSystem(trialName + "/initialState");

	ifstream inCfg("settings.cfg", ios::binary);
	ofstream outCfg(trialName + "/settings.cfg", ios::binary);

	outCfg << inCfg.rdbuf();

}

void system::estimateCompletion(PSim::timer* tmr) {
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

void system::writeToStream(string path, double value) {
	//Output the number of clusters with time.
	std::ofstream myFile(path,
			std::ios_base::app | std::ios_base::out);
	myFile << currentTime << " " << value << "\n";
	myFile.close();
}

void system::writeSystemState(PSim::timer* tmr) {
	//Update the console.
	std::string outName = std::to_string(int(std::round(currentTime)));
	PSim::util::setTerminalColour(PSim::Colour::Cyan);
	std::cout << "\n\n" << "Writing: " << outName << ".txt";
	PSim::util::setTerminalColour(PSim::Colour::Normal);

	//Write the recovery image.
	std::string dirName = trialName + "/snapshots/time-" + outName;
	mkdir(dirName.c_str(), 0777);
	writeSystem(dirName + "/recovery");

	//Write the XYZ image.
	std::string movName = trialName + "/movie/system-" + outName;
	writeSystemXYZ(movName);

	//Calculate the perfomance.
	estimateCompletion(tmr);

	//Average coordination number and potential.
	int totCoor = 0;
	int totEAP = 0;
	for (int i = 0; i < nParticles; i++) {
		totCoor += particles[i]->getCoorNumber();
		totEAP += particles[i]->getPotential();
	}

	double eap = (totEAP / double(nParticles));
	double nClust = avgClusterSize(outXYZ);
	double avgCoor = double(totCoor) / double(nParticles);
	double meanR2 = meanDisplacement();

	//Output the current system statistics.
	std::cout << "\n<Coor>: " << avgCoor << " - Total Coor: " << totCoor
			<< "\n";
	std::cout << "<EAP>: " << eap << "\n";
	std::cout << "<N>/Nc: " << nClust << "\n";
	std::cout << "<R^2>: " << meanR2 << "\n";
	std::cout << "Temperature: " << getTemperature() << "\n";

	writeToStream(trialName + "/clustGraph.txt", nClust);
	writeToStream(trialName + "/potGraph.txt", eap);
	writeToStream(trialName + "/coorGraph.txt", avgCoor);
	writeToStream(trialName + "/meanR2Graph.txt", meanR2);
}

}

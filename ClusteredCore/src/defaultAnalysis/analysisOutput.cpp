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

/********************************************//**
 *-----------------SYSTEM OUTPUT------------------
 ************************************************/

void analysisManager::writeSystemXYZ(particle** particles, int nParticles, int outXYZ, double currentTime, std::string name) {
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
		myFile.close();
	}
}

void analysisManager::writeSystem(particle** particles, int nParticles, std::string name) {
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

void analysisManager::writeInitTemp(particle** particles, int nParticles) {
	double v2 = 0.0;
	//Get V^2 for each particle.
	for (int i = 0; i < nParticles; i++) {
		v2 += particles[i]->getVX() * particles[i]->getVX();
		v2 += particles[i]->getVY() * particles[i]->getVY();
		v2 += particles[i]->getVZ() * particles[i]->getVZ();
	}
	//Average v2.
	double vAvg = v2 / ceil(float(nParticles));
	double temp = (vAvg / 3.0);
	//
	chatterBox.consoleMessage("Temp: " + tos(temp) + " m/k",3);
}

void analysisManager::writeToStream(double currentTime, string path, double value) {
	//Output the number of clusters with time.
	std::ofstream myFile(path,
			std::ios_base::app | std::ios_base::out);
	myFile << currentTime << " " << value << "\n";
	myFile.close();
}

void analysisManager::writeSystemState(particle** particles, int nParticles, double currentTime) {
	//Update the console.
	bool outXYZ = true;

	std::string outName = std::to_string(int(std::round(currentTime)));

	util::writeTerminal("Writing: " + outName + ".txt", Colour::Cyan);

	//Write the recovery image.
	std::string dirName = trialName + "/snapshots/time-" + outName;
	mkdir(dirName.c_str(), 0777);
	writeSystem(particles, nParticles, dirName + "/recovery");

	//Write the XYZ image.
	std::string movName = trialName + "/movie/system-" + outName;
	writeSystemXYZ(particles, nParticles, outXYZ, currentTime, movName);

	//Average coordination number and potential.
	int totalCoor = 0;
	float totalPot = 0;
	for (int i = 0; i < nParticles; i++) {
		totalCoor += particles[i]->getCoorNumber();
		totalPot += particles[i]->calculatePotential();
	}

	double pot = totalPot/double(nParticles);
	double nClust = writeClusters(particles, nParticles, currentTime, outXYZ);
	double avgCoor = double(totalCoor) / double(nParticles);
	double meanR2 = meanDisplacement(particles, nParticles);
	double trackedMeanR2 = trackedDisplacement(particles, nParticles);

	//Output the current system statistics.
	chatterBox.consoleMessage("<Coor>: " + tos(avgCoor) + " - Total Coor: " + tos(totalCoor));
	chatterBox.consoleMessage("<EAP>: " + tos(pot));
	chatterBox.consoleMessage("<N>/Nc: " + tos(nClust));
	chatterBox.consoleMessage("<R^2>: " + tos(meanR2));
	chatterBox.consoleMessage("Temperature: " + tos(getTemperature(particles, nParticles)));

	writeToStream(currentTime, trialName + "/clustGraph.txt", nClust);
	writeToStream(currentTime, trialName + "/coorGraph.txt", avgCoor);
	writeToStream(currentTime, trialName + "/meanR2Graph.txt", meanR2);
	writeToStream(currentTime, trialName + "/trackedMeanR2Graph.txt", trackedMeanR2);
}

}

#ifndef DEFAULTANALYSIS_H_
#define DEFAULTANALYSIS_H_

#include "particle.h"
#include "interfaces/IAnalysisManager.h"

using namespace std;

namespace PSim {

class analysisManager : public PSim::IAnalysisManager {

private:
	// Number of times a particles has crossed a periodic boundary.
	int* xPBC;
	int* yPBC;
	int* zPBC;
	// Initial starting position of each particle.
	float* xStart;
	float* yStart;
	float* zStart;
	// System parameters
	int counter;
	int boxSize;
	string trialName = "";

	/**
	 * Update the [x,y,z]PBC variables.
	 * @param i
	 * @param particles
	 */
	void updateParticlePBC(int i, particle** particles);
	void updateTracker(particle** particles, int nParticles);
	void writeInitTemp(particle** particles, int nParticles);
	/**
	 * Gets the temperature of the system. Should be equal to the temperature defined in the config file.
	 * @param particles
	 * @param nParticles
	 * @return
	 */
	double getTemperature(particle** particles, int nParticles);
	/**
	 * Calculate the average square displacement for a single integration step.
	 * @param particles
	 * @param nParticles
	 * @return
	 */
	double meanDisplacement(particle** particles, int nParticles);
	/**
	 * Calculate the average total square displacement.
	 * @param particles
	 * @param nParticles
	 * @return
	 */
	double trackedDisplacement(particle** particles, int nParticles);
	/**
	 * Histogram of particle coordination number.
	 * @param particles
	 * @param nParticles
	 */
	void coordinationHistogram(particle** particles, int nParticles);
	/**
	 * Write a data value to a given file.
	 * @param currentTime
	 * @param path
	 * @param value
	 */
	void writeToStream(double currentTime, string path, double value);
	/**
	 * Initial the [x,y,z]Start variables.
	 * @param particles
	 * @param nParticles
	 */
	void initDisplacementTracker(particle** particles, int nParticles);
	/**
	 * Write the entire particle system to file. Used to reloading.
	 * @param particles
	 * @param nParticles
	 * @param name
	 */
	void writeSystem(particle** particles, int nParticles, std::string name);
	void clusterSizeHistogram(std::vector<std::vector<particle*>> clusterPool);
	void writeSystemState(particle** particles, int nParticles, double currentTime);
	std::vector<std::vector<particle*>> findClusters(particle** particles, int nParticles);
	int writeClusters(std::vector<std::vector<particle*>> clusterPool, double currentTime, int xyz);
	void writeSystemXYZ(particle** particles, int nParticles, int outXYZ, double currentTime,string name);
	void clusterSizeHistogram(particle** particles, int nParticles) { clusterSizeHistogram(findClusters(particles, nParticles)); }
	int writeClusters(particle** particles, int nParticles, double currentTime, int xyz) { return writeClusters(findClusters(particles, nParticles),currentTime,xyz); }

public:
	analysisManager(string tName, systemState* state);
	void postAnalysis(std::queue<std::string>* tests, particle** particles, systemState* state);
	void writeInitialState(particle** particles, systemState* state);
	void writeFinalState(particle** particles, systemState* state);
	void writeRunTimeState(particle** particles, systemState* state);
};
}

#endif /* DEFAULTANALYSIS_H_ */

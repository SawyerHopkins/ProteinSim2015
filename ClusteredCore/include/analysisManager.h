#ifndef DEFAULTANALYSIS_H_
#define DEFAULTANALYSIS_H_

#include "particle.h"
#include "interfaces/IAnalysisManager.h"

using namespace std;

namespace PSim {

class analysisManager : public PSim::IAnalysisManager {

private:
	string trialName = "";

	double getTemperature(particle** particles, int nParticles);
	double meanDisplacement(particle** particles, int nParticles);
	std::vector<std::vector<particle*>> findClusters(particle** particles, int nParticles);
	int writeClusters(std::vector<std::vector<particle*>> clusterPool, double currentTime, int xyz);
	int writeClusters(particle** particles, int nParticles, double currentTime, int xyz) { return writeClusters(findClusters(particles, nParticles),currentTime,xyz); }
	void clusterSizeHistogram(std::vector<std::vector<particle*>> clusterPool);
	void clusterSizeHistogram(particle** particles, int nParticles) { clusterSizeHistogram(findClusters(particles, nParticles)); }
	void coordinationHistogram(particle** particles, int nParticles);
	void writeSystemXYZ(particle** particles, int nParticles, int outXYZ, double currentTime,string name);
	void writeSystem(particle** particles, int nParticles, std::string name);
	void writeInitTemp(particle** particles, int nParticles);
	void writeToStream(double currentTime, string path, double value);
	void writeSystemState(particle** particles, int nParticles, int outXYZ, double currentTime);

public:
	analysisManager(string tName) { trialName = tName; }
	void postAnalysis(std::queue<std::string>* tests, particle** particles, int nParticles);
	void writeInitialState(particle** particles, int nParticles);
	void writeFinalState(particle** particles, int nParticles);
	void writeRunTimeState(particle** particles, int nParticles, int outXYZ, double currentTime);
};
}

#endif /* DEFAULTANALYSIS_H_ */

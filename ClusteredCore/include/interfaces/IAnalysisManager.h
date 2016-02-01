#ifndef IANALYSIS_H_
#define IANALYSIS_H_

namespace PSim {

class IAnalysisManager {

public:

	virtual ~IAnalysisManager() {};
	virtual void postAnalysis(std::queue<std::string>* tests, particle** particles, systemState* state) = 0;
	virtual void writeInitialState(particle** particles, systemState* state) = 0;
	virtual void writeFinalState(particle** particles, systemState* state) = 0;
	virtual void writeRunTimeState(particle** particles, systemState* state) = 0;

};

}

#endif /* IANALYSIS_H_ */

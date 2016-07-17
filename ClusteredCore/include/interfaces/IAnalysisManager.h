#ifndef IANALYSIS_H_
#define IANALYSIS_H_

namespace PSim {

/**
 * Contract for analysis managers
 */
class IAnalysisManager {

public:

	virtual ~IAnalysisManager() {};
	/** Analysis routines to be run via the '-a' flag */
	virtual void postAnalysis(std::queue<std::string>* tests, particle** particles, systemState* state) = 0;
	/** Triggered before integration. */
	virtual void writeInitialState(particle** particles, systemState* state) = 0;
	/** Triggered after integration completed. */
	virtual void writeFinalState(particle** particles, systemState* state) = 0;
	/** Triggered base on outputFreq configuration option. */
	virtual void writeRunTimeState(particle** particles, systemState* state) = 0;

};

}

#endif /* IANALYSIS_H_ */

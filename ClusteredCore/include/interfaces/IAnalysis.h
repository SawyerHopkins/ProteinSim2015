#ifndef IANALYSIS_H_
#define IANALYSIS_H_

namespace PSim {

class IAnalysis {

public:

	virtual ~IAnalysis() {};
	virtual void runAnalysis() = 0;

};

}

#endif /* IANALYSIS_H_ */

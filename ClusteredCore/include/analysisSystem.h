#ifndef ANALYSIS_SYSTEM_H
#define ANALYSIS_SYSTEM_H

#include "system.h"

namespace PSim {

class AnalysisSystem: public system {
public:
	AnalysisSystem(config* cfg, std::string sysState, std::string timeStamp, IAnalysisManager* analysisInterface);
};

}

#endif

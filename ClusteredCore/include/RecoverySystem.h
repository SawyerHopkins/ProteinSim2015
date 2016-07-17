#ifndef RECOVERY_SYSTEM_H
#define RECOVERY_SYSTEM_H

#include "system.h"

namespace PSim {

class RecoverySystem: public system {
public:
	RecoverySystem(config* cfg, std::string sysState,
			std::string timeStamp, PSim::IIntegrator* sysInt,
			PSim::defaultForceManager* sysFcs);
};
}

#endif

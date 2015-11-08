#ifndef IGRID_H_
#define IGRID_H_
#include "IMoveable.h"

class IGrid {

public:
	virtual ~IGrid();

	virtual void addMember() = 0;

};


#endif /* IGRID_H_ */

#ifndef IGRID_H_
#define IGRID_H_
#include "particle.h"

class IGrid {

public:
	// Create and destroy grid.
	virtual ~IGrid();
	virtual void initCell() = 0;
	// Manage members
	virtual void addMember(particle* p) = 0;
	virtual void removeMember(particle* p) = 0;
	// Expose members
	virtual void getBegin() = 0;
	virtual void getEnd() = 0;
	virtual void getMember() = 0;
	// Expose neighbor cells.
	virtual void getFirstNeighbor() = 0;
	virtual void getLastNeighbor() = 0;
};


#endif /* IGRID_H_ */

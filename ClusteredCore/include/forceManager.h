#ifndef FORCE_H
#define FORCE_H
#include <omp.h>
#include "config.h"
#include "particle.h"
#include "interfaces/IForce.h"

namespace PSim {


/********************************************//**
 *----------------FORCE MANAGEMENT----------------
 ***********************************************/

/**
 * @class forces
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file force.h
 * @brief Management system for a collection of forces.
 */
class defaultForceManager {
private:

	//A vector of all forces in the system.
	std::vector<IForce*> flist;
	//Flagged if flist contains a time dependant force.
	bool timeDependent;

public:

	/**
	 * @brief Creates the force management system.
	 */
	defaultForceManager();
	/**
	 * @brief Releases the management system.
	 */
	~defaultForceManager();

	/**
	 * @brief Adds a force to the management system.
	 * @param f The force to add. Must implement IForce interface.
	 */
	void addForce(IForce* f);

	/**
	 * @brief Find the net force on all particles in the system.
	 * @param nPart The number of particles in the system.
	 * @param boxSize The size of the system.
	 * @param time The current system time.
	 * @param cells The system cell manager.
	 * @param items The particles in the system.
	 */
	void getAcceleration(double* sortedParticles, double* particleForce, vector<tuple<int,int>>* particleHashIndex, vector<tuple<int,int>>* cellStartEnd, systemState* state);

	void getPostRoutine(double* sortedParticles, double* particleForce, vector<tuple<int,int>>* particleHashIndex, vector<tuple<int,int>>* cellStartEnd, systemState* state);
	/**
	 * @brief Checks if the system contains a time dependent force.
	 * @return True if time dependent. False otherwise.
	 */
	bool isTimeDependent() {
		return timeDependent;
	}

	/**
	 * @brief Set the number of threads for OMP to use
	 * @param num Number of threads.
	 */
	void setNumThreads(int num) {
		if (num > 0) {
			omp_set_num_threads(num);
		}
	}
	/**
	 * @brief Set the dynamic/static mode of operation.
	 * @param num 0 for static. num > 0 for dynamics.
	 */
	void setDynamic(int num) {
		omp_set_dynamic(num);
	}
	/**
	 * @brief Set the default OMP target device.
	 * @param num Device number.
	 */
	void setDevice(int num) {
	}
	; //omp_set_default_device(num); }

	//Iterators

	/**
	 * @brief Gets the beginning iterator of the force list.
	 * @return flist.begin().
	 */
	std::vector<IForce*>::iterator getBegin() {
		return flist.begin();
	}
	/**
	 * @brief Gets the end iterator of the force list.
	 * @return flist.end();
	 */
	std::vector<IForce*>::iterator getEnd() {
		return flist.end();
	}

};

}

#endif // FORCE_H

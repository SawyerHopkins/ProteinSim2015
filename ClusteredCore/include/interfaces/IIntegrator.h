/*
 * IIntegrator.h
 *
 *  Created on: Nov 8, 2015
 *      Author: sawyer
 */

#ifndef IINTEGRATOR_H_
#define IINTEGRATOR_H_

namespace PSim
{
/**
 * @class I_integrator
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file integrator.h
 * @brief Creates an abstract parent class (interface) for a generic integrator.
 */
class IIntegrator {

protected:

	std::string name;

public:

	//Header Version.
	static const int version = 1;

	virtual ~IIntegrator() {};

	/**
	 * @brief Integrates to the next system state.
	 * @param time The current system time.
	 * @param dt The amount of time to advance.
	 * @param nParticles The number of particles in the system.
	 * @param boxSize The size of the system.
	 * @param cells The cells manager for the system.
	 * @param items The particles in the the system.
	 * @param f The force acting on the system.
	 * @return Return 0 for no error.
	 */
	virtual int nextSystem(PSim::particle** items, systemState* state)=0;

	/**
	 * @brief Get the name of the integrator for logging purposes.
	 * @return
	 */
	std::string getName() {
		return name;
	}

};
}

#endif /* IINTEGRATOR_H_ */

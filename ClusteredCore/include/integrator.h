#ifndef INTEGRATOR_H
#define INTEGRATOR_H
#include "forceManager.h"
#include "interfaces/IIntegrator.h"
#include "fastRandom.h"
#include <omp.h>

namespace PSim {
/********************************************//**
 *-----------BROWNIAN MOTION INTEGRATOR-----------
 ************************************************/

/**
 * @class brownianIntegrator
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file integrator.h
 * @brief Integrator for brownian dynamics.
 */
class brownianIntegrator: public IIntegrator {

private:

	//System variables
	double mass;
	double temp;
	int memSize;

	//Variables vital to the force.
	double gamma;
	double dt;
	double dtInv;
	double y;

	//Variables for integrator.
	//See GUNSTEREN AND BERENDSEN 1981 EQ 2.6
	double coEff0;
	double coEff1;
	double coEff2;
	double coEff3;

	//The previous kick.
	double * memX;
	double * memY;
	double * memZ;

	//The correlation to the previous kick.
	double * memCorrX;
	double * memCorrY;
	double * memCorrZ;

	//Random number storage for parallel compute
	fastRandom** rndGens;

	//Gaussian width.
	double sig1;
	double sig2;
	double corr;
	double dev;

	//Tempature vars;
	double goy2;
	double goy3;
	double hn;
	double gn;

	//Update Flags;
	int velFreq;
	int velCounter;

	//Random number seed;
	int seed;

	/**
	 * @brief Gets the width of the random gaussians according to G+B 2.12
	 * @param gdt gamma * dT
	 * @return
	 */
	double getWidth(double gdt);

public:

	/**
	 * @brief Constructs the brownian motion integrator.
	 * @param cfg The address of the configuration file reader.
	 * @return Nothing
	 */
	brownianIntegrator(configReader::config* cfg);
	/**
	 * @brief Deconstructs the integrator.
	 * @return Nothing.
	 */
	~brownianIntegrator();

	/**
	 * @brief Normal coefficents for high gamma.
	 * @param cfg Config file reader.
	 */
	void setupHigh(configReader::config* cfg);
	/**
	 * @brief Series expanded coefficents for low gamma.
	 * @param cfg Config file reader.
	 */
	void setupLow(configReader::config* cfg);
	/**
	 * @brief Special case coefficents.
	 * @param cfg Config file reader.
	 */
	void setupZero(configReader::config* cfg);

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
	int nextSystem(PSim::particle** items, systemState* state);

	/**
	 * @brief Integrates to the next system state.
	 * @param time The current system time.
	 * @param dt The amount of time to advance.
	 * @param nParticles The number of particles in the system.
	 * @param boxSize The size of the system.
	 * @param items The particles in the system.
	 * @param f The force acting on the system.
	 * @return Return 0 for no error.
	 */
	int firstStep(PSim::particle** items, systemState* state);

	/**
	 * @brief Integrates to the next system state.
	 * @param time The current system time.
	 * @param dt The amount of time to advance.
	 * @param nParticles The number of particles in the system.
	 * @param boxSize The size of the system.
	 * @param items The particles in the system.
	 * @param f The force acting on the system.
	 * @return Return 0 for no error.
	 */
	int normalStep(PSim::particle** items, systemState* state);

	/**
	 * @brief Integrates the velocity when desired.
	 * @param items The particles in the system.
	 * @param i The working index.
	 * @param xNew0,yNew0,zNew0 The newly integrated position.
	 * @param dt The amount of time to advance.
	 * @param boxSize The size of the system.
	 */
	void velocityStep(PSim::particle** items, int i, type3<double>* newPos0, double dt, double boxSize);

};

}

#endif // VERLET_H

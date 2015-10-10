#ifndef INTEGRATOR_H
#define INTEGRATOR_H
#include "force.h"
#include <curand_kernel.h>

namespace integrators
{
	/********************************************//**
	*--------------INTEGRATOR INTERFACE--------------
	************************************************/

	/**
	 * @class I_integrator
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file integrator.h
	 * @brief Creates an abstract parent class (interface) for a generic integrator.
	 */
	class I_integrator
	{
		public:

			//Header Version.
			static const int version = 1;

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
			__device__
			virtual void nextSystem(float *time, float *dt, int *nParticles, int *boxSize, simulation::particle* items)=0;

			/**
			 * @brief Loads the initial values into the object.
			 * @param vars The array of values to load.
			 */
			__device__
			virtual void cudaTest()=0;
			/**
			 * @brief Performs an intial test on the integrator.
			 * @param vars
			 */
			__device__
			virtual void cudaLoad(float* vars)=0;
	};

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
	class brownianIntegrator
	{
		private:

			//System variables
			float mass;
			float temp;
			int memSize;

			//Variables vital to the force.
			float gamma;
			float dt;
			float dtInv;
			float y;

			//Variables for integrator.
			//See GUNSTEREN AND BERENDSEN 1981 EQ 2.6
			float coEff0;
			float coEff1;
			float coEff2;
			float coEff3;

			//The previous kick.
			float* memX;
			float* memY;
			float* memZ;

			//The correlation to the previous kick.
			float* memCorrX;
			float* memCorrY;
			float* memCorrZ;

			//Random number generator.
			curandStateXORWOW_t* devStates;
			int rSeed;

			//Gaussian width.
			float sig1;
			float sig2;
			float corr;
			float dev;

			//Tempature vars;
			float goy2;
			float goy3;
			float hn;
			float gn;

			//Update Flags;
			int velFreq;
			int velCounter;

			/**
			 * @brief Gets the width of the random gaussians according to G+B 2.12
			 * @param gdt gamma * dT
			 * @return 
			 */
			__host__ __device__
			float getWidth(float gdt);

		public:

			__device__
			brownianIntegrator() {};
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
			__host__ __device__
			void setupHigh();
			/**
			 * @brief Series expanded coefficents for low gamma.
			 * @param cfg Config file reader.
			 */
			__host__ __device__
			void setupLow();
			/**
			 * @brief Special case coefficents.
			 * @param cfg Config file reader.
			 */
			__host__ __device__
			void setupZero();

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
			__device__
			void nextSystem(float* time, float* dt, int* nParticles, int* boxSize, simulation::particle* items);

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
			__device__
			void firstStep(float time, float dt, int nParticles, int boxSize, simulation::particle* items);

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
			__device__
			void normalStep(float time, float dt, int nParticles, int boxSize, simulation::particle* items);

			/**
			 * @brief Integrates the velocity when desired.
			 * @param items The particles in the system.
			 * @param i The working index.
			 * @param xNew0,yNew0,zNew0 The newly integrated position.
			 * @param dt The amount of time to advance.
			 * @param boxSize The size of the system.
			 */
			__device__
			void velocityStep(simulation::particle* items, int i, float xNew0, float yNew0, float zNew0, float dt, float boxSize);

			/**
			 * @brief Performs an intial test on the integrator.
			 */
			__device__
			void cudaTest();
			/**
			 * @brief Loads the initial values into the object.
			 * @param vars The array of values to load.
			 */
			__device__
			void cudaLoad(float* vars);
	};
}

#endif // VERLET_H
#ifndef SYSTEM_H
#define SYSTEM_H
#include "integrator.h"
#include <dlfcn.h>

namespace simulation
{
	/**
	 * @class system
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file system.h
	 * @brief Frontend wrapper for running simulations.
	 */
	class system
	{
		private:

			/********************************************//**
			*-----------------SYSTEM VARIABLES---------------
			 ***********************************************/

			//Trial name
			std::string trialName;

			//Information about the system.
			int nParticles;
			int *d_nParticles;
			float concentration;
			int boxSize;
			int *d_boxSize;
			int cellSize;
			int *d_cellSize;
			int cellScale;
			int *d_cellScale;
			float temp;
			float currentTime;
			float *d_currentTime;
			float dTime;
			float *d_dTime;
			int particlesPerCell;
			int *d_particlesPerCell;
			int numCells;
			int *d_numCells;

			//Settings flags
			int outputFreq;
			int outXYZ;
			float cycleHour;

			//Random number seed.
			int seed;

			//System entities.
			particle* particles;
			particle* d_particles;
			cell* cells;

			//System integrator.
			integrators::brownianIntegrator* integrator;
			physics::IForce* sysForces;
			void* forceLib;

			/********************************************//**
			*-------------------SYSTEM INIT------------------
			 ***********************************************/

			/**
			 * @brief Creates an initial uniform distribution of particles.
			 * @param r The radius of the particles
			 * @param m The mass of the particles.
			 */
			void initParticles(float r, float m);
			/**
			 * @brief Creates a maxwell distribution of velocities for the system temperature.
			 * @param gen The random generator the initalize particles.
			 * @param distribution The distribution for the particles.
			 */
			void maxwellVelocityInit(std::mt19937* gen, std::uniform_real_distribution<float>* distribution);
			/**
			 * @brief Fixes any particle overlap in the random initalization.
			 * @param gen The random generator the initalize particles.
			 * @param distribution The distribution for the particles.
			 */
			void initCheck(std::mt19937* gen, std::uniform_real_distribution<float>* distribution);
			/**
			 * @brief Get input for working directory. Create if needed.
			 * @return The working directory 
			 */
			std::string runSetup();
			/**
			 * @brief Check that the provided path is a valid directory.
			 * @param path Directory path
			 * @return True is the path is valid.
			 */
			bool checkDir(std::string path);

			void checkCuda(std::string name);

		public:

			//Header Version.
			static const int version = 1;

			/********************************************//**
			*---------------SYSTEM CONSTRUCTION--------------
			 ***********************************************/

			/**
			 * @brief Constructs the particle system.
			 * @return Nothing.
			 */
			system(configReader::config* cfg, integrators::brownianIntegrator* sysInt, physics::IForce* sysFcs, void* frcLib, int nParts);
			/**
			 * @brief Destructs the particle system.
			 * @return Nothing.
			 */
			~system();

			/********************************************//**
			*-----------------SYSTEM GETTERS-----------------
			 ***********************************************/

			/**
			 * @brief Gets the number of particles in the system.
			 * @return Number of particles.
			 */
			 int getNParticles() const { return nParticles; }
			/**
			 * @brief Gets the length of the system box.
			 * @return length of the system box.
			 */
			 int getBoxSize() const { return boxSize; }
			/**
			 * @brief Gets the length of a system cell.
			 * @return cellSize.
			 */
			 int getCellSize() const { return cellSize; }

			/********************************************//**
			*-----------------SYSTEM HANDLING----------------
			 ***********************************************/

			/**
			 * @brief Runs the system.
			 * @param endTime When to stop running the simulation.
			 */
			void run(float endTime);

			/********************************************//**
			*------------------SYSTEM OUTPUT-----------------
			 ***********************************************/

			/**
			 * @brief Writes the temperature of the system.
			 */
			void writeInitTemp();
			/**
			 * @brief Writes the position of a particle.
			 * @param index The index of the particle to write.
			 */
			void writePosition(int index) { particles[index].writePosition(); }
			/**
			 * @brief Writes the system to file.
			 * @param name The name of the file to write to.
			 */
			void writeSystem(std::string name);
			/**
			 * @brief Write the initial system parameters.
			 */
			void writeSystemInit();
			/**
			 * @brief Writes the varies that define the system state. Average potential. Average coordination number. Temperature. Cluster size.
			 */
			void writeSystemState(debugging::timer* tmr);
			/**
			 * @brief Outputs the system as XYZ for visualization purposes.
			 * @param name File name
			 */
			void writeSystemXYZ(std::string name);

			/********************************************//**
			*-----------------SYSTEM RECOVERY----------------
			 ***********************************************/

			/**
			 * @brief Recover a system state from output files.
			 * @param settings The location of the settings file.
			 * @param sysState The location of the system file.
			 */
			void loadFromFile(std::string settings, std::string sysState);

			/********************************************//**
			*-----------------SYSTEM ANALYSIS----------------
			 ***********************************************/

			/**
			 * @brief Get the number of clusters in the system.
			 * @return Return the number of clusters
			 */
			int numClusters(int xyz);
			/**
			 * @brief Returns the temperature of the system.
			 * @return 
			 */
			float getTemperature();

			/********************************************//**
			 *---------------VERSION CONTROL-----------------
			 ***********************************************/

			/**
			 * @brief Gets the flagged version of cell.h for debugging.
			 * @return 
			 */
			int getCellVersion() { return simulation::cell::version; }
			/**
			 * @brief Gets the flagged version of config.h for debugging.
			 * @return 
			 */
			int getConfigVersion() { return configReader::config::version; }
			/**
			 * @brief Gets the flagged version of error.h for debugging.
			 * @return 
			 */
			int getErrorVersion() { return debugging::error::version; }
			/**
			 * @brief Gets the flagged version of particle.h for debugging.
			 * @return 
			 */
			int getParticleVersion() { return simulation::particle::version; }
			/**
			 * @brief Gets the flagged version of timer.h for debugging.
			 * @return 
			 */
			int getTimerVersion() { return debugging::timer::version; }
			/**
			 * @brief Gets the flagged version of utilities.h for debugging.
			 * @return 
			 */
			int getUtilitiesVersion() { return utilities::util::version; }
			/**
			 * @brief Gets the flagged version of system.h for debugging.
			 * @return 
			 */
			int getSystemVersion() { return version; }
	};
}

#endif // SYSTEM_H

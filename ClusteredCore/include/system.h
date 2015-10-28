#ifndef SYSTEM_H
#define SYSTEM_H
#include "integrator.h"

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
			double concentration;
			int boxSize;
			int cellSize;
			int cellScale;
			double temp;
			double currentTime;
			double dTime;

			//Settings flags
			int outputFreq;
			int outXYZ;
			double cycleHour;

			//Random number seed.
			int seed;

			//System entities.
			particle** particles;
			cell**** cells;

			//System integrator.
			integrators::I_integrator* integrator;
			physics::forces* sysForces;

			/********************************************//**
			*-------------------SYSTEM INIT------------------
			 ***********************************************/

			/**
			 * @brief Creates the cell system.
			 * @param numCells The number of cells to be created.
			 * @param scale The number of cells in each dimension. (numCells^1/3)
			 */
			void initCells(int scale);
			/**
			 * @brief Creates an initial uniform distribution of particles.
			 * @param r The radius of the particles
			 * @param m The mass of the particles.
			 */
			void initParticles(double r, double m);
			/**
			 * @brief Creates a maxwell distribution of velocities for the system temperature.
			 * @param gen The random generator the initalize particles.
			 * @param distribution The distribution for the particles.
			 */
			void maxwellVelocityInit(std::mt19937* gen, std::uniform_real_distribution<double>* distribution);
			/**
			 * @brief Fixes any particle overlap in the random initalization.
			 * @param gen The random generator the initalize particles.
			 * @param distribution The distribution for the particles.
			 */
			void initCheck(std::mt19937* gen, std::uniform_real_distribution<double>* distribution);
			/**
			 * @brief Get input for working directory. Create if needed.
			 * @return The working directory 
			 */
			static std::string runSetup();
			/**
			 * @brief Check that the provided path is a valid directory.
			 * @param path Directory path
			 * @return True is the path is valid.
			 */
			static bool checkDir(std::string path);

			/********************************************//**
			*-----------------SYSTEM HANDLING----------------
			 ***********************************************/

			/**
			 * @brief Updates the cells that the particles are located in.
			 * @return 
			 */
			void updateCells();

		public:

			//Header Version.
			static const int version = 1;

			/********************************************//**
			*---------------SYSTEM CONSTRUCTION--------------
			 ***********************************************/

			/**
			 * @brief Default constructor used for loading system from recovery file.
			 */
			system() {};
			/**
			 * @brief Constructs the particle system.
			 * @return Nothing.
			 */
			system(configReader::config* cfg, integrators::I_integrator* sysInt, physics::forces* sysFcs);
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
			const int getNParticles() const { return nParticles; }
			/**
			 * @brief Gets the length of the system box.
			 * @return length of the system box.
			 */
			const int getBoxSize() const { return boxSize; }
			/**
			 * @brief Gets the length of a system cell.
			 * @return cellSize.
			 */
			const int getCellSize() const { return cellSize; }

			/********************************************//**
			*-----------------SYSTEM HANDLING----------------
			 ***********************************************/

			/**
			 * @brief Runs the system.
			 * @param endTime When to stop running the simulation.
			 */
			void run(double endTime);

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
			void writePosition(int index) { particles[index]->writePosition(); }
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
			 * @brief Find the number of line in a file.
			 */
			static int particlesInFile(std::string sysState, std::string timeStamp);
			/**
			 * @brief Read in the sysConfig file.
			 */
			static void readSettings(system* oldSys, configReader::config* cfg);
			/**
			 * @brief Load a system with particle data from file.
			 */
			static int readParticles(system* oldSys, std::string sysState, std::string timeStamp, int bsize);
			/**
			 * @brief Attempt to create the correct directory for rewind.
			 */
			static void createRewindDir(system* oldSys);
			/**
			 * @brief Recover a system state from output files.
			 */
			static system* loadFromFile(configReader::config* cfg, std::string sysState, std::string timeStamp, integrators::I_integrator* sysInt, physics::forces* sysFcs);
			/**
			 * @brief Recover to a hulk state (only particle system initialized).
			 */
			static system* loadAnalysis(configReader::config* cfg, std::string sysState, std::string timeStamp);

			/********************************************//**
			*-----------------SYSTEM ANALYSIS----------------
			 ***********************************************/

			void createInteractionsTable();
			void analysisManager(std::queue<std::string>* tests);
			/**
			 *
			 *@brief gets an array of clusters.
			 *
			 */
			std::vector<std::vector<particle*>> findClusters();
			/**
			 * @brief Get the number of clusters in the system.
			 * @return Return the number of clusters
			 */
			int avgClusterSize(std::vector<std::vector<particle*>> clusterPool, int xyz);
			int avgClusterSize(int xyz) { return avgClusterSize(findClusters(), xyz); }
			/**
			 *
			 * @brief Outputs a coordination number histogram of a specific snapshot.
			 *
			 */
			void coordinationHistogram();
			/**
			 *
			 * @brief Outputs a cluster size histogram of a specific snapshot.
			 *
			 */
			void clusterSizeHistogram(std::vector<std::vector<particle*>> clusterPool);
			void clusterSizeHistogram() { clusterSizeHistogram(findClusters() ); }
			/**
			 * @brief Returns the temperature of the system.
			 * @return 
			 */
			double getTemperature();
			/**
			 *
			 * @brief Sets a new time step. Use with caution.
			 *
			 */
			double setdTime(double dt) { 
				double old = dTime;
				dTime = dt;
				return old; }
			/**
			 *
			 * @brief Sets a new temperature parameter. Use with caution.
			 *
			 */
			double setSysTemp(double val) { 
				double old = temp;
				temp = val;
				return old; }
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
			 * @brief Gets the flagged version of force.h before debugging.
			 * @return 
			 */
			int getForceVersion() { return physics::IForce::version; }
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
			 * @brief Gets the flagged version of the integrator for debugging..
			 * @return  
			 */
			int getIntegratorVersion() { return integrators::I_integrator::version; }
			/**
			 * @brief Gets the flagged version of system.h for debugging.
			 * @return 
			 */
			int getSystemVersion() { return version; }

	};

}

#endif // SYSTEM_H

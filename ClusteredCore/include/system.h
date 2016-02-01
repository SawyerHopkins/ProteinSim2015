#ifndef SYSTEM_H
#define SYSTEM_H
#include "integrator.h"
#include "analysisManager.h"

using namespace std;

namespace PSim {

/**
 * @class system
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file system.h
 * @brief Frontend wrapper for running simulations.
 */
class system {

private:

	/********************************************//**
	 *-----------------SYSTEM VARIABLES---------------
	 ***********************************************/

	//Trial name
	std::string trialName;

	//Information about the system.
	systemState state;

	//Settings flags
	double cycleHour;

	//System entities.
	particle** particles;
	PeriodicGrid**** cells;

	//System integrator.
	PSim::IIntegrator* integrator;
	PSim::defaultForceManager* sysForces;
	PSim::IAnalysisManager* analysis;

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
	void maxwellVelocityInit(std::mt19937* gen,
			std::uniform_real_distribution<double>* distribution);
	/**
	 * @brief Fixes any particle overlap in the random initalization.
	 * @param gen The random generator the initalize particles.
	 * @param distribution The distribution for the particles.
	 */
	void initCheck(std::mt19937* gen,
			std::uniform_real_distribution<double>* distribution);
	/**
	 * @brief Get input for working directory. Create if needed.
	 * @return The working directory
	 */
	static std::string dirPrompt();
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


	/********************************************//**
	 *----------------REFACTORING DUMP---------------
	 ***********************************************/

	void verifyPath();
	void estimateCompletion(PSim::timer* tmr);
	void writeToStream(string path, double value);
	void allocateCells(int scale);
	void setCellNeighbors(int scale);
	void assignCellParticles();
	void setSystemConstants(configReader::config* cfg,
			PSim::IIntegrator* sysInt, PSim::defaultForceManager* sysFcs);

public:

	//Header Version.
	static const int version = 1;

	/********************************************//**
	 *---------------SYSTEM CONSTRUCTION--------------
	 ***********************************************/

	/**
	 * @brief Default constructor used for loading system from recovery file.
	 */
	system() {
	}
	;
	/**
	 * @brief Constructs the particle system.
	 * @return Nothing.
	 */
	system(configReader::config* cfg, PSim::IIntegrator* sysInt,
			PSim::defaultForceManager* sysFcs);
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
	const int getNParticles() const {
		return state.nParticles;
	}
	/**
	 * @brief Gets the length of the system box.
	 * @return length of the system box.
	 */
	const int getBoxSize() const {
		return state.boxSize;
	}
	/**
	 * @brief Gets the length of a system cell.
	 * @return cellSize.
	 */
	const int getCellSize() const {
		return state.cellSize;
	}

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
	 * @brief Writes the position of a particle.
	 * @param index The index of the particle to write.
	 */
	void writePosition(int index) {
		particles[index]->writePosition();
	}
	/**
	 * @brief Write the initial system parameters.
	 */
	void writeSystemInit();

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
	static int readParticles(system* oldSys, std::string sysState,
			std::string timeStamp, int bsize);
	/**
	 * @brief Attempt to create the correct directory for rewind.
	 */
	static void createRewindDir(system* oldSys);
	/**
	 * @brief Recover a system state from output files.
	 */
	static system* loadFromFile(configReader::config* cfg, std::string sysState,
			std::string timeStamp, PSim::IIntegrator* sysInt,
			PSim::defaultForceManager* sysFcs);
	/**
	 * @brief Recover to a hulk state (only particle system initialized).
	 */
	static system* loadAnalysis(configReader::config* cfg, std::string sysState,
			std::string timeStamp, IAnalysisManager* analysisInterface);

	/********************************************//**
	 *-----------------SYSTEM ANALYSIS----------------
	 ***********************************************/

	/**
	 * @return Gets the default analysis interface for the system.
	 */
	PSim::IAnalysisManager* defaultAnalysisInterface(int nParticles, int boxSize) { return new PSim::analysisManager(trialName, &state); }
	/**
	 * @brief Runs the tests and analysis provided in the input string.
	 * @param tests
	 */
	void analysisManager(std::queue<std::string>* tests) { analysis->postAnalysis(tests, particles, &state); }
	/**
	 * Return the interactions list for each particle.
	 */
	void createInteractionsTable();
	/**
	 *
	 * @brief Sets a new time step. Use with caution.
	 *
	 */
	double setdTime(double dt) {
		double old = state.dTime;
		state.dTime = dt;
		return old;
	}
	/**
	 *
	 * @brief Sets a new temperature parameter. Use with caution.
	 *
	 */
	double setSysTemp(double val) {
		double old = state.temp;
		state.temp = val;
		return old;
	}
	/********************************************//**
	 *---------------VERSION CONTROL-----------------
	 ***********************************************/

	/**
	 * @brief Gets the flagged version of cell.h for debugging.
	 * @return
	 */
	int getCellVersion() {
		return PSim::PeriodicGrid::version;
	}
	/**
	 * @brief Gets the flagged version of config.h for debugging.
	 * @return
	 */
	int getConfigVersion() {
		return configReader::config::version;
	}
	/**
	 * @brief Gets the flagged version of error.h for debugging.
	 * @return
	 */
	int getErrorVersion() {
		return PSim::error::version;
	}
	/**
	 * @brief Gets the flagged version of force.h before debugging.
	 * @return
	 */
	int getForceVersion() {
		return PSim::IForce::version;
	}
	/**
	 * @brief Gets the flagged version of particle.h for debugging.
	 * @return
	 */
	int getParticleVersion() {
		return PSim::particle::version;
	}
	/**
	 * @brief Gets the flagged version of timer.h for debugging.
	 * @return
	 */
	int getTimerVersion() {
		return PSim::timer::version;
	}
	/**
	 * @brief Gets the flagged version of utilities.h for debugging.
	 * @return
	 */
	int getUtilitiesVersion() {
		return PSim::util::version;
	}
	/**
	 * @brief Gets the flagged version of the integrator for debugging..
	 * @return
	 */
	int getIntegratorVersion() {
		return PSim::IIntegrator::version;
	}
	/**
	 * @brief Gets the flagged version of system.h for debugging.
	 * @return
	 */
	int getSystemVersion() {
		return version;
	}

};

}

#endif // SYSTEM_H

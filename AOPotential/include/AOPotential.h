/********************************************//**
*------------------AO POTENTIAL------------------
************************************************/
 #include "forceManager.h"
 #include "utilities.h"

using namespace PSim;
using namespace std;

/**
 * @class AOPotential
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file force.h
 * @brief AO Potential.
 */
class AOPotential : public PSim::IForce
{

private:

		//Variables vital to the force.
		double kT;
		double cutOff;
		double dt;

		//Secondary variables.
		double coEff1;
		double coEff2;

		//Potential variables.
		double a1;
		double a2;
		double a3;

	public:

		/**
		 * @brief Creates an new AO Potential.
		 * @param cfg The address of the configuration file reader.
		 */
		AOPotential(configReader::config* cfg);
		/**
		 * @brief Releases the force from memory.
		 */
		~AOPotential();

		/**
		 * @brief Get the force from the AO Potential.
		 * @param index The index particle to calculated the force on.
		 * @param nPart The number of particles in the system.
		 * @param boxSize The size of the system.
		 * @param time The current system time.
		 * @param itemCell The cell containing the index particle.
		 * @param items All particles in the system.
		 */
		void getAcceleration(int index, PSim::PeriodicGrid* itemCell, PSim::particle** items, systemState* state);
		/**
		 * @brief Flag for a force dependent time.
		 * @return True for time dependent. False otherwise. 
		 */
		bool isTimeDependent() { return false; }
		/**
		 * @brief Checks for particle interation between the index particle and all particles in the provided cell.
		 * @param boxSize The size of the system.
		 * @param time The current system time.
		 * @param index The particle to find the force on.
		 * @param itemCell The cell to check for interactions in.
		 */
		void iterCells(int boxSize, double time, PSim::particle* index, PSim::PeriodicGrid* itemCell);

		void quench(){};

};

//Class factories.
extern "C" PSim::IForce* getForce(configReader::config* cfg)
{
	return new AOPotential(cfg);
}

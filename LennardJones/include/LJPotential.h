#include "forceManager.h"
#include "utilities.h"

using namespace PSim;
using namespace std;

/**
 * @class Yukawa
 * @author Sawyer Hopkins
 * @date 07/26/15
 * @file force.h
 * @brief Yukawa Potential
 */
class LennardJones : public PSim::IForce
{

private:

		//Variables vital to the force.
		double kT;
		double yukStr;
		int ljNum;
		double cutOff;
		double cutOffSquared;
		double debyeLength; //k
		double debyeInv;
		double mass; // m
		double radius; // r
		bool output;

	public:

		/**
		 * @brief Creates an new AO Potential.
		 * @param cfg The address of the configuration file reader.
		 */
		LennardJones(configReader::config* cfg);
		/**
		 * @brief Releases the force from memory.
		 */
		~LennardJones();

		/**
		 * @brief Get the force from the AO Potential.
		 * @param index The index particle to calculated the force on.
		 * @param nPart The number of particles in the system.
		 * @param boxSize The size of the system.
		 * @param time The current system time.
		 * @param itemCell The cell containing the index particle.
		 * @param items All particles in the system.
		 */
		void getAcceleration(int index, int nPart, int boxSize, double time, PeriodicGrid* itemCell, PSim::particle** items);
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
		void iterCells(int boxSize, double time, PSim::particle* index, PeriodicGrid* itemCell);
		
		void quench();

};

//Class factories.
extern "C" PSim::IForce* getForce(configReader::config* cfg)
{
	return new LennardJones(cfg);
}

#include "force.h"

/**
 * @class Yukawa
 * @author Sawyer Hopkins
 * @date 07/26/15
 * @file force.h
 * @brief Yukawa Potential
 */
class LennardJones : public physics::IForce
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
		double gamma; // g^2
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
		__device__
		void getAcceleration(int index, int nPart, int boxSize, double time, simulation::cell* itemCell, simulation::particle* item);
		/**
		 * @brief Checks for particle interation between the index particle and all particles in the provided cell.
		 * @param boxSize The size of the system.
		 * @param time The current system time.
		 * @param index The particle to find the force on.
		 * @param itemCell The cell to check for interactions in.
		 */
		__device__
		void iterCells(int boxSize, double time, simulation::particle* index, simulation::cell* itemCell);

};

//Class factories.
extern "C" physics::IForce* getForce(configReader::config* cfg)
{
	return new LennardJones(cfg);
}
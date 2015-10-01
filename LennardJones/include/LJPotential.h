#include "force.h"
#include <stdio.h> 
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
		float kT;
		float yukStr;
		int ljNum;
		float cutOff;
		float cutOffSquared;
		float debyeLength; //k
		float debyeInv;
		float mass; // m
		float gamma; // g^2
		float radius; // r
		bool output;

	public:

		__device__
		LennardJones(){};
		/**
		 * @brief Creates an new AO Potential.
		 * @param cfg The address of the configuration file reader.
		 */
		LennardJones(configReader::config* cfg);
		/**
		 * @brief Releases the force from memory.
		 */
		~LennardJones();

		__device__
		void cudaLoad(float * vars);

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
		void getAcceleration(int index, int nPart, int boxSize, float time, simulation::cell* itemCell, simulation::particle* item);
		/**
		 * @brief Checks for particle interation between the index particle and all particles in the provided cell.
		 * @param boxSize The size of the system.
		 * @param time The current system time.
		 * @param index The particle to find the force on.
		 * @param itemCell The cell to check for interactions in.
		 */
		__device__
		void iterCells(int boxSize, float time, simulation::particle* index, simulation::cell* itemCell);

		__device__
		void cudaTest(){ printf("%d\n",1234); }


};

__global__
void cudaForce(physics::IForce* force)
{
	force = new LennardJones();
}

__global__
void cudaTest(physics::IForce* force)
{
	((LennardJones*)force)->cudaTest();
}

__global__
void cudaLoad(physics::IForce* force, float * vars)
{
	((LennardJones*)force)->cudaLoad(vars);
}

__global__
void runForce(int index, int nPart, int boxSize, float time, simulation::cell* itemCell, simulation::particle* items, physics::IForce* force)
{
	((LennardJones*)force)->getAcceleration(index,nPart,boxSize,time,itemCell,items);
}

extern "C" void showError(std::string name)
{
	std::string err = cudaGetErrorString(cudaGetLastError());
	if (err != "no error")
	{
		utilities::util::writeTerminal("IFORCE: " + name + "-" + err + "\n", utilities::Colour::Red);
	}
	else
	{
		utilities::util::writeTerminal("IFORCE: " + name + "-" + err + "\n", utilities::Colour::Green);
	}
}

//Class factories.
extern "C" physics::IForce* getForce(configReader::config* cfg)
{
	return new LennardJones(cfg);
}

extern "C" void getCudaForce(physics::IForce* force)
{
	cudaForce<<<1,1>>>(force);
	cudaTest<<<1,1>>>(force);
	cudaDeviceSynchronize();
	std::string err = cudaGetErrorString(cudaGetLastError());
	showError("getCudaForce");
}

extern "C" void getCudaTest(physics::IForce* force)
{
	cudaTest<<<1,1>>>(force);
	cudaDeviceSynchronize();
	std::string err = cudaGetErrorString(cudaGetLastError());
	showError("getCudaTest");
}

extern "C" void getCudaLoad(physics::IForce* force, float * vars)
{
	cudaLoad<<<1,1>>>(force,vars);
	cudaDeviceSynchronize();
	showError("getCudaLoad");
}

extern "C" void getRunForce(int index, int nPart, int boxSize, float time, simulation::cell* itemCell, simulation::particle* items, physics::IForce* force)
{
	runForce<<<nPart,1>>>(index, nPart, boxSize, time, itemCell, items, force);
	cudaDeviceSynchronize();
	showError("getRunForce");
}
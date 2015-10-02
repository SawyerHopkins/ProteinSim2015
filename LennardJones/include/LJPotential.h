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
		LennardJones(float* vars);
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
		void getAcceleration(int* nPart, int* boxSize, int* cellScale ,float* time, simulation::cell* cells, simulation::particle* items);
		/**
		 * @brief Checks for particle interation between the index particle and all particles in the provided cell.
		 * @param boxSize The size of the system.
		 * @param time The current system time.
		 * @param index The particle to find the force on.
		 * @param itemCell The cell to check for interactions in.
		 */
		__device__
		void iterCells(int* boxSize, simulation::particle* index, simulation::cell* itemCell);

		__device__
		void cudaTest(){ printf("%f\n",kT); }
};

__global__
void buildKernel(physics::IForce** force, float* vars)
{
	(*force) = new LennardJones(vars);
}

__global__
void testKernel(physics::IForce** force)
{
	(*force)->cudaTest();
}

__global__
void accelerationKernel(physics::IForce** force, int* nPart, int* boxSize, int* cellScale ,float* time, simulation::cell* cells, simulation::particle* items)
{
	(*force)->getAcceleration(nPart, boxSize, cellScale, time, cells, items);
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

//Class factories.
extern "C" void getCudaForce(physics::IForce** force, float* vars)
{
	buildKernel<<<1,1>>>(force,vars);
	cudaDeviceSynchronize();
	std::string err = cudaGetErrorString(cudaGetLastError());
	//showError("buildKernel");
}

//Class factories.
extern "C" void runCudaTest(physics::IForce** force)
{
	testKernel<<<1,1>>>(force);
	cudaDeviceSynchronize();
	std::string err = cudaGetErrorString(cudaGetLastError());
	//showError("testKernel");
}

//Class factories.
extern "C" void runAcceleration(physics::IForce** force, int* nPart, int* boxSize, int* cellScale ,float* time, simulation::cell* cells, simulation::particle* items, int numThreads)
{
	accelerationKernel<<<numThreads,1>>>(force, nPart, boxSize, cellScale, time, cells, items);
	cudaDeviceSynchronize();
	std::string err = cudaGetErrorString(cudaGetLastError());
	//showError("runAcceleration");
}
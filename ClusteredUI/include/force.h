#ifndef FORCE_H
#define FORCE_H
#include <omp.h>
#include "cell.h"
#include "config.h"

namespace physics
{
	/********************************************//**
	*-----------------FORCE INTERFACE----------------
	 ***********************************************/

	/**
	 * @class IForce
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file force.h
	 * @brief A generic force container.
	 */
	class IForce
	{
		public:

			int size;

			//Header Version.
			static const int version = 1;

			/**
			 * @brief Virtual methods for forces of various parameters.
			 * @param index The index particle to find the force on.
			 * @param nPart The number of particles in the system.
			 * @param boxSize The size of the system.
			 * @param time The current system time.
			 * @param itemCell The cell containing the index particle. 
			 * @param items All particles in the system.
			 */
			__device__
			virtual void getAcceleration(int* nPart, int* boxSize, int* cellScale ,float* time, simulation::cell* cells, simulation::particle* items)=0;

			/**
			 * @brief Run any precalculation tests on the device to ensure it has properly loaded.
			 */
			__device__
			virtual void cudaTest()=0;
	};

	/** Create the host force */
	typedef IForce* create_Force(configReader::config*);
	typedef void create_CudaForce(physics::IForce**, float *);
	typedef void cuda_Test(physics::IForce**);
	typedef void cuda_Acceleration(physics::IForce**, int*, int*, int*, float*, simulation::cell*, simulation::particle*, int numThreads);
	/********************************************//**
	*----------------FORCE MANAGEMENT----------------
	 ***********************************************/

	/**
	 * @class forces
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file force.h
	 * @brief Management system for a collection of forces. This force is currently orphaned in cuda branch.
	 */
	class forces
	{
		private:

			//A vector of all forces in the system.
			IForce* flist;
			//Flagged if flist contains a time dependant force.
			bool timeDependent;

		public:

			/**
			 * @brief Creates the force management system.
			 */
			forces(IForce* add);
			/**
			 * @brief Releases the management system.
			 */
			~forces();
			/**
			 * @brief Checks if the system contains a time dependent force.
			 * @return True if time dependent. False otherwise.
			 */
			__host__ __device__
			bool isTimeDependent() { return timeDependent; }
			/**
			 * @brief Get the list of forces.
			 * @return 
			 */
			__device__
			IForce* getForce() { return flist; }
	};
}

#endif // FORCE_H

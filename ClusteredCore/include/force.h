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

		protected:

			std::string name;

		public:

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
			virtual void getAcceleration(int index, int nPart, int boxSize, double time, simulation::cell* itemCell, simulation::particle** items)=0;

			/**
			 * @brief Flag for a force dependent time.
			 * @return True for time dependent. False otherwise. 
			 */
			__device__ __host__
			virtual bool isTimeDependent()=0;

			/**
			 * @brief Get the name of the force for logging purposes.
			 * @return 
			 */
			__device__ __host__
			std::string getName() { return name; }

	};

	typedef IForce* create_Force(configReader::config*);

	/********************************************//**
	*----------------FORCE MANAGEMENT----------------
	 ***********************************************/

	/**
	 * @class forces
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file force.h
	 * @brief Management system for a collection of forces.
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
			forces();
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

			__device__
			IForce* getForce() { return flist; }

	};

}

#endif // FORCE_H

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
			virtual void getAcceleration(int index, int nPart, int boxSize, double time, simulation::cell* itemCell, simulation::particle** items)=0;

			/**
			 * @brief Flag for a force dependent time.
			 * @return True for time dependent. False otherwise. 
			 */
			virtual bool isTimeDependent()=0;

			/**
			 * @brief Allows for systematic control of force variables.
			 */
			virtual void quench()=0;

			/**
			 * @brief Get the name of the force for logging purposes.
			 * @return 
			 */
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
			std::vector<IForce*> flist;
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
			 * @brief Adds a force to the management system.
			 * @param f The force to add. Must implement IForce interface.
			 */
			void addForce(IForce* f);

			/**
			 * @brief Find the net force on all particles in the system.  
			 * @param nPart The number of particles in the system.
			 * @param boxSize The size of the system.
			 * @param time The current system time.
			 * @param cells The system cell manager.
			 * @param items The particles in the system.
			 */
			void getAcceleration(int nPart, int boxSize, double time, simulation::cell**** cells, simulation::particle** items);

			/**
			 * @brief Checks if the system contains a time dependent force.
			 * @return True if time dependent. False otherwise.
			 */
			bool isTimeDependent() { return timeDependent; }

			/**
			 * @brief Set the number of threads for OMP to use
			 * @param num Number of threads.
			 */
			void setNumThreads(int num) { if (num > 0) {omp_set_num_threads(num);} }
			/**
			 * @brief Set the dynamic/static mode of operation.
			 * @param num 0 for static. num > 0 for dynamics.
			 */
			void setDynamic(int num) { omp_set_dynamic(num); }
			/**
			 * @brief Set the default OMP target device.
			 * @param num Device number.
			 */
			void setDevice(int num) {}; //omp_set_default_device(num); }

			//Iterators

			/**
			 * @brief Gets the beginning iterator of the force list.
			 * @return flist.begin().
			 */
			std::vector<IForce*>::iterator getBegin() { return flist.begin(); }
			/**
			 * @brief Gets the end iterator of the force list.
			 * @return flist.end();
			 */
			std::vector<IForce*>::iterator getEnd() { return flist.end(); }

	};

}

#endif // FORCE_H

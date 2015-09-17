#ifndef CELL_H
#define CELL_H
#include "particle.h"
#include <thrust/pair.h>

namespace simulation
{

	/**
	 * @class cell
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file cell.h
	 * @brief Cells for the particle grid.
	 */
	class cell
	{

		private:

			//The particles in the cell.
			//std::map<int,particle*> members;
			cell* neighbors[27];
			//std::vector<cell*> neighbors;

		public:

			//Header Version.
			static const int version = 1;
			int maxMem;
			particle** members;

			//Constructor and Destructor
			__device__
			cell();
			__device__
			~cell();

			//Cell neighbors
			cell* top;
			cell* bot;
			cell* left;
			cell* right;
			cell* front;
			cell* back;
			
			/**
			 * @brief Find the index of the particle in the members array.
			 * @param p
			 * @return 
			 */
			__device__
			int findIndex(particle* p);
			/**
			 * @brief Gets the member at specific index.
			 * @param key
			 * @return 
			 */
			__device__
			const particle* getMember(int key) { return members[key]; }
			/**
			 * @brief Gets the iterator to the first neighboring cell.
			 * @return 
			 */
			__device__
			const cell* getNeighbor(int i) { return neighbors[i]; }
			/**
			 * @brief Creates a vector containing points to all adjacent cells.
			 */
			 __device__
			void createNeighborhood();

	};

}

#endif // CELL_H

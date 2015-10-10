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

			cell* neighbors[27];

		public:

			//Header Version.
			static const int version = 1;
			int maxMem;
			int gridCounter;
			int index;
			particle** members;

			//Constructor and Destructor
			__host__ __device__
			cell(int cellParts);
			__host__ __device__
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
			__host__ __device__
			int findIndex(particle* p);
			/**
			 * @brief Gets the member at specific index.
			 * @param key
			 * @return 
			 */
			__host__ __device__
			const particle* getMember(int key) { return members[key]; }
			/**
			 * @brief Gets the iterator to the first neighboring cell.
			 * @return 
			 */
			__host__ __device__
			cell* getNeighbor(int i) { return neighbors[i]; }
			/**
			 * @brief Creates a vector containing points to all adjacent cells.
			 */
			__host__ __device__
			void createNeighborhood();
	};
}

#endif // CELL_H

/*The MIT License (MIT)

Copyright (c) <2015> <Sawyer Hopkins>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include "force.h"

namespace physics
{
	/********************************************//**
	*-----------------FORCE MANAGEMENT---------------
	 ***********************************************/
	/**
	 * @brief Find the net force on all particles in the system.  
	 * @param nPart The number of particles in the system.
	 * @param boxSize The size of the system.
	 * @param time The current system time.
	 * @param cells The system cell manager.
	 * @param items The particles in the system.
	 */
	__global__
	void getAcceleration(int *nPart, int *boxSize, double *time, simulation::cell**** cells, simulation::particle** items, IForce* flist)
	{
		int index = 0;
		for (index; index < nPart; index++)
		{
			//Resets the force on the particle.
			items[index]->nextIter();

			simulation::particle* p = items[index];
			simulation::cell* itemCell = cells[p->getCX()][p->getCY()][p->getCZ()];

			//Iterates through all forces.
			flist->getAcceleration(index, *nPart, *boxSize, *time, itemCell, items);
		}
	}

}

namespace integrators
{

	__global__
	void brownianIntegrator::nextSystem(double *time, double *dt, int *nParticles, int *boxSize, simulation::cell**** cells, simulation::particle** items, physics::forces* f, I_integrator* inter)
	{
		inter->nextSystem(time, dt, nParticles, boxSize, cells, items, f)
	}

}

namespace simulation
{
	/**
	 * @brief Creates the cell system.
	 * @param numCells The number of cells to be created.
	 * @param scale The number of cells in each dimension. (numCells^1/3)
	 */
	__global__
	void system::initCells(int numCells, int scale, simulation::cell**** cells, simulation::particle** d_particles)
	{

		//Create the cells.
		cells = new cell***[scale];
		for(int i=0; i < scale; i++)
		{
			cells[i] = new cell**[scale];
			for(int j=0; j < scale; j++)
			{
				cells[i][j] = new cell*[scale];
				for(int k=0; k < scale; k++)
				{
					cells[i][j][k] = new cell();
				}
			}
		}

		//Set the cell neighbors.
		for(int x=0; x < scale; x++)
		{
			for(int y=0; y < scale; y++)
			{
				for(int z=0; z < scale; z++)
				{
					int left = x-1;
					int right = x+1;
					int top = y-1;
					int bot = y+1;
					int front = z-1;
					int back = z+1;

					if (x == 0)
					{
						left = (scale-1);
					}
					else if (x == (scale-1))
					{
						right = 0;
					}

					if (y == 0)
					{
						top = (scale-1);
					}
					else if (y == (scale-1))
					{
						bot = 0;
					}

					if (z == 0)
					{
						front = (scale-1);
					}
					else if (z == (scale-1))
					{
						back = 0;
					}

					//Set the 6 principle cells next the current cell.
					//Access diagonals through combinations of these six.
					cells[x][y][z]->left = cells[left][y][z];
					cells[x][y][z]->right = cells[right][y][z];
					cells[x][y][z]->top = cells[x][top][z];
					cells[x][y][z]->bot = cells[x][bot][z];
					cells[x][y][z]->front = cells[x][y][front];
					cells[x][y][z]->back = cells[x][y][back];
				}
			}
		}

		//Check the cell neighborhood mapping.
		for(int x=0; x < scale; x++)
		{
			for(int y=0; y < scale; y++)
			{
				for(int z=0; z < scale; z++)
				{
					cells[x][y][z]->createNeighborhood();
				}
			}
		}

		//Assign the particle to their starting cell.
		for(int i=0; i < nParticles; i++)
		{
			int cx = d_particles[i]->getX() / cellSize;
			int cy = d_particles[i]->getY() / cellSize;
			int cz = d_particles[i]->getZ() / cellSize;

			//Tell the particle what cell its in, then add to cell.
			d_particles[i]->setCell(cx,cy,cz);
			cells[cx][cy][cz]->addMember(&(d_particles[i]));

		}

	}
}
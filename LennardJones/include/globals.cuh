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

#ifndef GLOBALS_H
#define GLOBALS_H

#include "integrator.h"
#include <stdio.h> 

/********************************************//**
*---------------CUDA INDEX MANAGMENT-------------
 ***********************************************/

/**
 * @brief Get the cell index from 0 to (numcells - 1);
 * @param x particle.cX
 * @param y particle.cY
 * @param z particle.cZ
 * @param scale the cell grid scale. numCells^(1/3)
 * @return 
 */
__device__
int getIndex(int x, int y, int z, int scale)
{
	return x + (y * scale) + (z * scale * scale);
}

/********************************************//**
*-----------CUDA INTEGRATION MANAGEMENT----------
 ***********************************************/

/**
 * @brief Starts the kernal to run the brownianIntegrator.
 * @param time Current system time.
 * @param dt Time step for the integrator to use.
 * @param nParticles Number of particles in the system.
 * @param boxSize Size of the system.
 * @param items The array of particles on device.
 * @param inter The integrator on device.
 */
__global__
void nextSystem(float* time, float* dt, int* nParticles, int* boxSize, simulation::particle* items, integrators::brownianIntegrator* inter)
{
	inter->nextSystem(time, dt, nParticles, boxSize, items);
}

/**
 * @brief Loads the integrator with the intial variables.
 * @param inter The integrator on device.
 * @param vars The variables to load.
 */
__global__
void loadIntegrator(integrators::brownianIntegrator* inter, float* vars)
{
	inter->cudaLoad(vars);
}

/**
 * @brief Runs an intial test on the device integrator.
 * @param inter The device integrator.
 */
__global__
void testIntegrator(integrators::brownianIntegrator* inter)
{
	inter->cudaTest();
}

/********************************************//**
*---------------CUDA CELL MANAGEMENT-------------
 ***********************************************/
/**
 * @brief Creates the cell system.
 * @param numCells The number of cells to be created.
 * @param scale The number of cells in each dimension. (numCells^1/3)
 */
__global__
void initCells(int numCells, int scale, simulation::cell* cells, int particlesPerCell)
{
	using namespace simulation;

	int x = blockIdx.x;
	int y = blockIdx.y;
	int z = blockIdx.z;

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

	int index = getIndex(x,y,z,scale);

	cells[index].left = &(cells[getIndex(left,y,z,scale)]);
	cells[index].right = &(cells[getIndex(right,y,z,scale)]);
	cells[index].top = &(cells[getIndex(x,top,z,scale)]);
	cells[index].bot = &(cells[getIndex(x,bot,z,scale)]);
	cells[index].front = &(cells[getIndex(x,y,front,scale)]);
	cells[index].back = &(cells[getIndex(x,y,back,scale)]);

	cells[index].index = index;
	cells[index].members = new particle*[particlesPerCell];
}

__global__
void createNeighborhoods(simulation::cell* cells)
{
	cells[blockIdx.x].createNeighborhood();
}

/**
 * @brief Creates the cell system.
 * @param numCells The number of cells to be created.
 * @param scale The number of cells in each dimension. (numCells^1/3)
 */
__global__
void updateCells(int* scale, int* size, simulation::cell* cells, simulation::particle* d_particles)
{
		int index = (blockDim.x * blockIdx.x) + threadIdx.x;

		//if (index >= nParticles) return;
		//New cell
		int cX = int( d_particles[index].getX() / float(*size) );
		int cY = int( d_particles[index].getY() / float(*size) );
		int cZ = int( d_particles[index].getZ() / float(*size) );

		if (cX > ((*scale)-1))
		{
			printf("Grid Size Overflow X --- particle: %d, cX: %d\n --- x: %f, x0: %f, fx: %f, fx0: %f\n", blockIdx.x, cX, d_particles[index].getX(), 
				d_particles[index].getX0(), d_particles[index].getFX(), d_particles[index].getFX0());
			debugging::error::throwCellBoundsError(cX,cY,cZ);
		}
		if (cY > ((*scale)-1))
		{
			printf("Grid Size Overflow Y --- particle: %d, cY: %d\n --- y: %f, y0: %f, fy: %f, fy0: %f\n", blockIdx.x, cY, d_particles[index].getY(), 
				d_particles[index].getY0(), d_particles[index].getFY(), d_particles[index].getFY0());
			debugging::error::throwCellBoundsError(cX,cY,cZ);
		}
		if (cZ > ((*scale)-1))
		{
			printf("Grid Size Overflow Z --- particle: %d, cZ: %d\n --- z: %f, z0: %f, fz: %f, fz0: %f\n", blockIdx.x, cZ, d_particles[index].getZ(), 
				d_particles[index].getZ0(), d_particles[index].getFZ(), d_particles[index].getFZ0());
			debugging::error::throwCellBoundsError(cX,cY,cZ);
		}

		d_particles[index].setCell(cX,cY,cZ);

		int cellIndex = getIndex(cX,cY,cZ,(*scale));

		//See cuda manual on particle simulations.
		//Create a thread unique index and add particle there.
		int j = atomicAdd( &(cells[cellIndex].gridCounter) , 1);

		//If we have to many particles per cell we are having a bad day.
		if (j >= 300)
		{
			printf("Cell Member Overflow --- gridCounter: %d, thread: %d\n", blockIdx.x, j);
		}
		//If this happens we are really having a bad day.
		else if (j < 0)
		{
			printf("Cell Member Underflow --- gridCounter: %d, thread: %d\n", blockIdx.x, j);
		}
		else
		{
			cells[cellIndex].members[j] = &(d_particles[index]);
		}
}

/**
 * @brief Reset the cell so that we reindex as particle zero. 
 * @param cells
 */
__global__
void resetCells(simulation::cell* cells)
{
	//Unlike Master branch, in cuda we clear cell members each time by doing this.
	cells[blockIdx.x].gridCounter = 0;
	//atomicInc(&(cells[blockIdx.x].gridCounter),0);
}

__global__
void incrementTime(float* dt, float* currentTime)
{
	*currentTime = *currentTime + *dt;
}


#endif // GLOBALS_H
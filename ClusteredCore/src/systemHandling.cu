/*The MIT License (MIT)

Copyright (c) [2015] [Sawyer Hopkins]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "system.h"

namespace simulation
{

	/********************************************//**
	*---------------PARTICLE HANDLING----------------
	************************************************/

	__global__
	void system::updateCells()
	{

		for (int index=0; index < nParticles; index++)
		{

			//New cell
			int cX = int( d_particles[index]->getX() / double(cellSize) );
			int cY = int( d_particles[index]->getY() / double(cellSize) );
			int cZ = int( d_particles[index]->getZ() / double(cellSize) );

			//Old cell
			int cX0 = d_particles[index]->getCX();
			int cY0 = d_particles[index]->getCY();
			int cZ0 = d_particles[index]->getCZ();

			//If cell has changed
			if ((cX != cX0) || (cY != cY0) || (cZ != cZ0))
			{

				if (cX > (cellScale-1))
				{
					debugging::error::throwCellBoundsError(cX,cY,cZ);
				}
				if (cY > (cellScale-1))
				{
					debugging::error::throwCellBoundsError(cX,cY,cZ);
				}
				if (cZ > (cellScale-1))
				{
					debugging::error::throwCellBoundsError(cX,cY,cZ);
				}

				//Remove from old. Add to new. Update particle address.
				cells[cX0][cY0][cZ0]->removeMember(d_particles[index]);
				cells[cX][cY][cZ]->addMember(d_particles[index]);
				d_particles[index]->setCell(cX,cY,cZ);
			}

		}

	}

}
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

#include "utilities.h"

namespace utilities
{
	__host__ __device__
	double util::safeMod(double val, double base)
	{
		//0 mod n is always zero
		if (val == 0)
		{
			return 0.0;
		}
		//if the particle is on the edge of the system.
		else if (val == base)
		{
			return 0.0;
		}
		//if the particle is outside the upper bounds.
		else if (val>base)
		{
			return (val-base);
		}
		//if the particle is outside the lower bounds.
		else if (val<0)
		{
			return (val+base);
		}
		//No problems return value.
		else
		{
			return val;
		}
	}

	__host__ __device__
	double util::safeMod0(double val0, double val, double base)
	{
		//The difference between the two values.
		double dx = val - val0;
		//If the values are further apart than half the system, use PBC.
		if (fabs(dx) > base/2 )
		{
			//Check which direction to implement PBC.
			if (dx < 0)
			{
				return val0-base;
			}
			else
			{
				return val0+base;
			}
		}
		else
		{
			return val0;
		}
	}

	__device__
	double util::pbcDist(double X,double Y, double Z,double X1, double Y1,double Z1,double L)
	{

		double dx = fabs(X-X1);
		double dy = fabs(Y-Y1);
		double dz = fabs(Z-Z1);

		//Check X direction.
		if(dx > L/2 )
		{
			dx-=L;
		}

		//Check Y direction.
		if(dy > L/2 )
		{
			dy-=L;
		}

		//Check Z direction.
		if(dz > L/2 )
		{
			dz-=L;
		}
		//Pythag for the distance.
		return (dx*dx)+(dy*dy)+(dz*dz);

	}

	__device__
	void util::unitVectorSimple(double dX, double dY, double dZ, double r, double (&acc)[3])
	{
		//Normalize by distance.
		acc[0]=dX/r;
		acc[1]=dY/r;
		acc[2]=dZ/r;
	}

	__device__
	void util::unitVectorAdv(double X,double Y, double Z,double X1, double Y1,double Z1,double (&acc)[3],double r,int L)
	{
		double dx,dy,dz;

		dx=X1-X; dy=Y1-Y; dz=Z1-Z;

		//Check X PBC.
		if(fabs(dx) > L/2)
		{
			if(dx<0)
			{
				dx=dx+L;
			}
			else
			{
				dx=dx-L;
			}
		}

		//Check Y PBC.
		if(fabs(dy) > L/2)
		{
			if(dy<0)
			{
				dy=dy+L;
			}
			else
			{
				dy=dy-L;
			}
		}

		//Check Z PBC.
		if(fabs(dz) > L/2)
		{
			if(dz<0)
			{
				dz=dz+L;
			}
			else
			{
				dz=dz-L;
			}
		}

		//Normalize by distance.
		dx=dx/r; dy=dy/r; dz=dz/r;
		acc[0]=dx; acc[1]=dy; acc[2]=dz;
	}

	__device__
	double util::powBinaryDecomp(double base, int exp)
	{
		double answer = 1;
		while(exp)
		{
			if (exp & 1)
			{
				answer *= base;
			}
			exp >>= 1;
			base *= base;
		}
		return answer;
	}
}
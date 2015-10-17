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
	__device__
	float util::safeMod(float val, int base)
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

	__device__
	float util::safeMod0(float val0, float val, int base)
	{
		//The difference between the two values.
		float dx = val - val0;
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

	__host__ __device__
	float util::pbcDist(float X, float Y, float Z, float X1, float Y1, float Z1, int L)
	{

		float dx = fabs(X-X1);
		float dy = fabs(Y-Y1);
		float dz = fabs(Z-Z1);

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
	void util::unitVectorSimple(float dX, float dY, float dZ, float r, float (&acc)[3])
	{
		//Normalize by distance.
		acc[0]=dX/r;
		acc[1]=dY/r;
		acc[2]=dZ/r;
	}

	__device__
	void util::unitVectorAdv(float X,float Y, float Z,float X1, float Y1,float Z1,float (&acc)[3],float r,int L)
	{
		float dx,dy,dz;

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
	float util::powBinaryDecomp(float base, int exp)
	{
		float answer = 1;
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
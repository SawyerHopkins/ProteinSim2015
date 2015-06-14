#ifndef PARTICLE_H
#define PARTICLE_H
#include <iostream>
#include "utilities.h"

namespace simulation
{

	class particle
	{

		/*-----------------------------------------*/
		/*-----------PARTICLE VARIABLES------------*/
		/*-----------------------------------------*/

		private:
			//Contains the current spacial information for x,y,z cordinates.
			double x;
			double y;
			double z;

			//Contains the previous spacial information for x,y,z cordinates.
			double x0;
			double y0;
			double z0;

			//Contains the velocity information for x,y,z cordinates.
			double vx;
			double vy;
			double vz;

			//Contains the current force information for the x,y,z cordinates.
			double fx;
			double fy;
			double fz;

			//Contains the previous force information for the x,y,z cordinates.
			double fx0;
			double fy0;
			double fz0;

			//Contains the radius of each particle.
			double r;

			//Contains the mass of each particle.
			double m;

			//Contains the current cell identification.
			int cx;
			int cy;
			int cz;
			int index;

		public:

			/*-----------------------------------------*/
			/*-----------SYSTEM CONSTRUCTION-----------*/
			/*-----------------------------------------*/

			//Constructor/Destructor
			particle();
			~particle();

			/*-----------------------------------------*/
			/*--------------SYSTEM GETTERS-------------*/
			/*-----------------------------------------*/

			//Getters for current position.
			const double getX() const { return x; }
			const double getY() const { return y; }
			const double getZ() const { return z; }

			//Getters for previous position.
			const double getX0() const { return x0; }
			const double getY0() const { return y0; }
			const double getZ0() const { return z0; }

			//Getters for velocity.
			const double getVX() const { return vx; }
			const double getVY() const { return vy; }
			const double getVZ() const { return vz; }
 
			//Getters for current force.
			const double getFX() const { return fx; }
			const double getFY() const { return fy; }
			const double getFZ() const { return fz; }

			//Getters for previous force.
			const double getFX0() const { return fx0; }
			const double getFY0() const { return fy0; }
			const double getFZ0() const { return fz0; }

			//Getters for containing cell.
			const double getCX() const { return cx; }
			const double getCY() const { return cy; }
			const double getCZ() const { return cz; }
			const double getIndex() const { return index; }

			//Getters for particle properties
			const double getRadius() const { return r; }
			const double getMass() const { return m; }
 
			/*-----------------------------------------*/
			/*--------------SYSTEM SETTERS-------------*/
			/*-----------------------------------------*/
 
			//Setters for current position.
			void setX(float val, float boxSize) { x0 = x; x = utilities::util::safeMod(val,boxSize); }
			void setY(float val, float boxSize) { y0 = y; y = utilities::util::safeMod(val,boxSize); }
			void setZ(float val, float boxSize) { z0 = z; z = utilities::util::safeMod(val,boxSize); }

			//Setters for velocity
			void setVX(float val) { vx = val; }
			void setVY(float val) { vy = val; }
			void setVZ(float val) { vz = val; }

			//Setters for current force.
			void setFX(float val) { fx0 = fx; fx = val; }
			void setFY(float val) { fy0 = fy; fy = val; }
			void setFZ(float val) { fz0 = fz; fz = val; }

			//Setter for containing cell.
			void setCell(int x, int y, int z) { cx = x; cy = y; cz = z; }
			void setIndex(int i) { index = i; }

			//Setters for particle properties
			void setRadius(float val) { r = val; }
			void setMass(float val) { m = val; }

			/*-----------------------------------------*/
			/*--------------SYSTEM OUTPUT--------------*/
			/*-----------------------------------------*/

			/**
			 * @brief Writes the position of the particle to the console.
			 */
			void writePosition() { std::cout << x << ", " << y << ", " << z << "\n"; }

	};

}

#endif // PARTICLE_H

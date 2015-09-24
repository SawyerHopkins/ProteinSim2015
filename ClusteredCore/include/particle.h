#ifndef PARTICLE_H
#define PARTICLE_H
#include "utilities.h"
namespace simulation
{

	/**
	 * @class particle
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file particle.h
	 * @brief 
	 */
	class particle
	{

		/********************************************//**
		*----------------SYSTEM VARIABLES----------------
		************************************************/

		private:

			//The particle identifier.
			int name;

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

			//Coordination number.
			int coorNumber;
			//Effective average potenital.
			double potential;

		public:

			//Header Version.
			static const int version = 1;

			/********************************************//**
			*--------------SYSTEM CONSTRUCTION---------------
			************************************************/

			__device__ __host__
			particle(){};
			//Constructor/Destructor
			/**
			 * @brief Creates a new particle with specified name.
			 * @param pid The name of the particle.
			 * @return 
			 */
			__device__ __host__
			particle(int pid);
			/**
			 * @brief Removes particle resources from memory.
			 * @return Nothing.
			 */
			__device__ __host__
			~particle();

			__device__ __host__
			void init(int pid);

			/********************************************//**
			*-----------------SYSTEM GETTERS-----------------
			************************************************/

			//Getters for current position.

			/**
			 * @brief Get the X position
			 * @return  exposes private variable x.
			 */
			__device__ __host__
			double getX() const { return x; }
			/**
			 * @brief Get the Y position
			 * @return  exposes private variable y.
			 */
			__device__ __host__
			double getY() const { return y; }
			/**
			 * @brief Get the Z position
			 * @return  exposes private variable z.
			 */
			__device__ __host__
			double getZ() const { return z; }

			//Getters for previous position.

			/**
			 * @brief Get the previous X position
			 * @return  exposes private variable x0.
			 */
			__device__ __host__
			double getX0() const { return x0; }
			/**
			 * @brief Get the previous Y position
			 * @return  exposes private variable y0.
			 */
			__device__ __host__
			double getY0() const { return y0; }
			/**
			 * @brief Get the previous X position
			 * @return  exposes private variable z0.
			 */
			__device__ __host__
			double getZ0() const { return z0; }

			//Getters for velocity.

			/**
			 * @brief Get the x velocity.
			 * @return  exposes private variable x.
			 */
			__device__ __host__
			double getVX() const { return vx; }
			/**
			 * @brief Get the y velocity.
			 * @return  exposes private variable y.
			 */
			__device__ __host__
			double getVY() const { return vy; }
			/**
			 * @brief Get the z velocity.
			 * @return  exposes private variable z.
			 */
			__device__ __host__
			double getVZ() const { return vz; }
 
			//Getters for current force.

			/**
			 * @brief Get the current x force.
			 * @return  exposes private variable fx.
			 */
			__device__ __host__
			double getFX() const { return fx; }
			/**
			 * @brief Get the current y force.
			 * @return  exposes private variable fy.
			 */
			__device__ __host__
			double getFY() const { return fy; }
			/**
			 * @brief Get the current z force.
			 * @return  exposes private variable fz.
			 */
			__device__ __host__
			double getFZ() const { return fz; }

			//Getters for previous force.

			/**
			 * @brief Get the previous x force.
			 * @return  exposes private variable fx0.
			 */
			__device__ __host__
			double getFX0() const { return fx0; }
			/**
			 * @brief Get the previous y force.
			 * @return  exposes private variable fy0.
			 */
			__device__ __host__
			double getFY0() const { return fy0; }
			/**
			 * @brief Get the previous z force.
			 * @return  exposes private variable fz0.
			 */
			__device__ __host__
			double getFZ0() const { return fz0; }

			//Getters for containing cell.

			/**
			 * @brief Get the x value of the containing cell.
			 * @return  exposes private variable cx.
			 */
			__device__ __host__
			int getCX() const { return cx; }
			/**
			 * @brief Get the y value of the containing cell.
			 * @return  exposes private variable cy.
			 */
			__device__ __host__ 
			int getCY() const { return cy; }
			/**
			 * @brief Get the z value of the containing cell.
			 * @return  exposes private variable cz.
			 */
			__device__ __host__ 
			int getCZ() const { return cz; }

			//Getters for particle properties

			/**
			 * @brief Get the radius of the particle.
			 * @return  exposes private variable r.
			 */
			__device__ __host__
			double getRadius() const { return r; }
			/**
			 * @brief Get the mass of the particle.
			 * @return  exposes private variable m.
			 */
			__device__ __host__
			double getMass() const { return m; }
			/**
			 * @brief Get the name of the particle.
			 * @return  exposes private variable name.
			 */
			__device__ __host__
			double getName() const { return name; }
			/**
			 * @brief Returns the coordination number.
			 * @return
			 */
			__device__ __host__
			int getCoorNumber() const { return coorNumber; }
			/**
			 * @brief Gets average potential.
			 * @return 
			 */
			 __device__ __host__ 
			int getPotential() const { return potential; }

 			/********************************************//**
			*-----------------SYSTEM SETTERS-----------------
			************************************************/

			/**
			 * @brief Set the x position of the particle. Handles PBC and updates x0.
			 * @param val The new x position. 
			 * @param boxSize The size of the system box.
			 */
			__device__ __host__
			void setX(double val, double boxSize);
			/**
			 * @brief Set the y position of the particle. Handles PBC and updates y0.
			 * @param val The new y position. 
			 * @param boxSize The size of the system box.
			 */
			__device__ __host__
			void setY(double val, double boxSize);
			/**
			 * @brief Set the z position of the particle. Handles PBC and updates z0.
			 * @param val The new z position. 
			 * @param boxSize The size of the system box.
			 */
			__device__ __host__
			void setZ(double val, double boxSize);
			/**
			 * @brief Set the position of the particle. Handles PBC and updates the previous value.
			 * @param xVal,yVal,zVal The new position. 
			 * @param boxSize The size of the system box.
			 */
			__device__ __host__
			void setPos(double xVal, double yVal, double zVal, double boxSize);
			/**
			 * @brief Set the x velocity.
			 * @param val The velocity to set.
			 */
			__device__ __host__
			void setVX(double val) { vx = val; }
			/**
			 * @brief Set the y velocity.
			 * @param val The velocity to set.
			 */
			__device__ __host__
			void setVY(double val) { vy = val; }
			/**
			 * @brief Set the z velocity.
			 * @param val The velocity to set.
			 */
			__device__ __host__
			void setVZ(double val) { vz = val; }
			/**
			 * @brief Adds the the current value of force.
			 * @param xVal,yVal,zVal The values of force to add.
			 * @param pot The potential of the force interaction.
			 * @param p The particle providing the force.
			 */
			__device__ __host__
			void updateForce(double xVal, double yVal, double zVal, double pot, particle* p, bool countPair = true);

			/**
			 * @brief Clears the current force and updates previous force. Resets potential, neighbors, and coordination number.
			 */
			__device__ __host__
			void nextIter();
			/**
			 * @brief Set the location of the containing cell.
			 * @param x,y,z The position variables of the containing cell.
			 */
			__device__ __host__
			void setCell(int x, int y, int z) { cx = x; cy = y; cz = z; }
			/**
			 * @brief Sets the radius of the particle.
			 * @param val Radius value.
			 */
			__device__ __host__
			void setRadius(double val) { r = val; }
			/**
			 * @brief Sets the mass of the particle.
			 * @param val Mass value.
			 */
			__device__ __host__
			void setMass(double val) { m = val; }


			/********************************************//**
			*------------------SYSTEM OUTPUT-----------------
			************************************************/

			/**
			 * @brief Writes the position of the particle to the console.
			 */
			void writePosition() { std::cout << x << ", " << y << ", " << z << "\n"; }

	};

}

#endif // PARTICLE_H

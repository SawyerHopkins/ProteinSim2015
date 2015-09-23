#ifndef ERROR_H
#define ERROR_H
#include <iostream>
#include <exception>
#include <cuda_runtime.h>

namespace debugging
{

	/**
	 * @class error
	 * @author Sawyer Hopkins
	 * @date 06/27/15
	 * @file error.h
	 * @brief Contains information for common runtime errors.
	 */
	class error : public std::exception
	{

		public:

			//Header Version.
			static const int version = 1;

			/**
			 * @brief Throw when system initial conditions cannot be resolved.
			 */
			__device__ __host__
			static void throwInitializationError();
			/**
			 * @brief Throw when trying to access a non existant cell.
			 * @param cx,cy,cz The cell to be accessed.
			 */
			__device__ __host__
			static void throwCellBoundsError(int cx, int cy, int cz);
			/**
			 * @brief Throw when the particle is outside the box after PBC check.
			 * @param x,y,z The position of the particle.
			 */
			__device__ __host__
			static void throwParticleBoundsError(int x, int y, int z);
			/**
			 * @brief Throw when particles are closer than the accepted tolerance.
			 * @param nameI Name of the index particle.
			 * @param nameJ Name of the force particle.
			 * @param r The distance between the particles.
			 */
			__device__ __host__
			static void throwParticleOverlapError(int nameI, int nameJ, double r);
			/**
			 * @brief Throw when force is given value NaN.
			 */
			__device__ __host__
			static void throwInfiniteForce();
			/**
			 * @brief Throw when input arguments are invalid.
			 */
			__device__ __host__
			static void throwInputError();

	};

}

#endif // ERROR_H

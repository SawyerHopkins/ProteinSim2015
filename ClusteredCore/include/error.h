#ifndef ERROR_H
#define ERROR_H
#include "defs.h"
#include <exception>
#include "structs/type3.h"

namespace PSim {

/**
 * @class error
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file error.h
 * @brief Contains information for common runtime errors.
 */
class error: public std::exception {

public:

	//Header Version.
	static const int version = 1;

	/**
	 * @brief Throw when system initial conditions cannot be resolved.
	 */
	static void throwInitializationError();
	/**
	 * @brief Throw when trying to access a non existant cell.
	 * @param cx,cy,cz The cell to be accessed.
	 */
	static void throwCellBoundsError(int cx, int cy, int cz);
	/**
	 * @brief Throw when the particle is outside the box after PBC check.
	 * @param x,y,z The position of the particle.
	 */
	static void throwParticleBoundsError(type3<double>* pos, int name);
	/**
	 * @brief Throw when particles are closer than the accepted tolerance.
	 * @param nameI Name of the index particle.
	 * @param nameJ Name of the force particle.
	 * @param r The distance between the particles.
	 */
	static void throwParticleOverlapError(int nameI, int nameJ, double r);
	/**
	 * @brief Throw when force is given value NaN.
	 */
	static void throwInfiniteForce();
	/**
	 * @brief Throw when input arguments are invalid.
	 */
	static void throwInputError();

};

}

#endif // ERROR_H

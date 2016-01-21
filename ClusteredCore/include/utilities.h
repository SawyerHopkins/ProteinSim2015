#ifndef UTILITIES_H
#define UTILITIES_H

#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <map>
#include <queue>
#include "error.h"
#include "timer.h"

using namespace std;

namespace PSim {

//Console colour codes.
#define __BLACK "\033[0;30m"
#define __RED "\033[0;31m"
#define __GREEN "\033[0;32m"
#define __BROWN "\033[0;33m"
#define __BLUE "\033[0;34m"
#define __MAGENTA "\033[0;35m"
#define __CYAN "\033[0;36m"
#define __GREY "\033[0;37m"
#define __NORMAL "\033[0m"

//Enum for chosing console text colour.
enum Colour {
	Black, Red, Green, Brown, Blue, Magenta, Cyan, Grey, Normal
};

/**
 * @class util
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file utilities.h
 * @brief Contains some useful math wrappers.
 */
class util {
public:

	//Header Version.
	static const int version = 1;

	/**
	 * @brief fmod can't do negative numbers so use this.
	 * @param val The value of the perform the division on.
	 * @param base The base of the modular divison.
	 * @return
	 */
	static double safeMod(double val, double base);

	/**
	 * @brief Looks for PBC check on original position.
	 * @param val0 Original position.
	 * @param val New position.
	 * @param base The size of the system.
	 * @return The new old position.
	 */
	static double safeMod0(double val0, double val, double base);

	/**
	 * @brief Method for getting distance between two points.
	 * @param X,Y,Z The position of the first particle
	 * @param X1,Y1,Z1 The position of the second particle.
	 * @param L The size of the system.
	 * @return The distance between the two particles.
	 */
	static double pbcDist(double X, double Y, double Z, double X1, double Y1,
			double Z1, double L);

	//
	/**
	 * @brief Shows the completed progress in the console.
	 * @param x0 The amount done
	 * @param n The total amount to be done.
	 * @param counter A logic counter.
	 * @param w The width of the progress bar. Default 50.
	 */
	static void loadBar(double x0, int n, int w = 50);

	/**
	 * @brief Normalizes the distances to create a unit vector in &acc[3].
	 * @param dX The distance in the X direction.
	 * @param dY The distance in the Y direction.
	 * @param dZ The distance in the Z direction.
	 * @param r The magnitude of the distance.
	 * @param acc The array hold the unit vectors.
	 */
	static void unitVectorSimple(double dX, double dY, double dZ, double r,
			double (&acc)[3]);

	/**
	 * @brief Alternative method for getting the normalized distance between two particles.
	 * @param X,Y,Z The position of the first particle
	 * @param X1,Y1,Z1 The position of the second particle.
	 * @param acc The array to hold the unit vectors
	 * @param r The distance between the particles.
	 * @param L The size of the box.
	 */
	static void unitVectorAdv(double X, double Y, double Z, double X1,
			double Y1, double Z1, double (&acc)[3], double r, int L);

	/**
	 * @brief Set the text terminal text colour.
	 * @param c The desired colour
	 */
	static void setTerminalColour(PSim::Colour c);
	/**
	 * @brief Writes a string to the terminal in the desired colour. Resets to default colour after writing.
	 * @param text The string to write.
	 * @param c The colour to write the string in.
	 */
	static void writeTerminal(std::string text, PSim::Colour c);
	/**
	 * @brief Clears lines text from the terminal including the current one.
	 * @param numLines The number of lines to clear.
	 */
	static void clearLines(int numLines);
	/**
	 * @brief A faster pow function with binary decomposition.
	 * @param base The base value
	 * @param exp The exponent.
	 * @return
	 */
	static double powBinaryDecomp(double base, int exp);

	/**
	 * @brief A fast method for generating a normal distribution.
	 * @param mu The mean.
	 * @param sigma The standard deviation.
	 * @return A gaussian random number with mean of mu and a standard deviation of sigma.
	 */
	static double g250(int seed, double mu = 0.0, double sigma = 1.0);

	/**
	 * @brief A fast method for generating a random number.
	 * @param iseed The seed to generate the random number from.
	 * @return
	 */
	static double psdrand(int iseed);

	/**
	 *
	 * @brief Attempts to pop an element of an input queue.
	 * @returns the popped element. Returns null if failed.
	 *
	 */
	static std::string tryPop(std::queue<std::string>* queue);

private:
	static void clearLine(int i, int numLines);
	static double safeMod0PBC(double dx, double val, double base);
};
}

#endif // UTILITIES_H

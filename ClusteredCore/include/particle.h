#ifndef PARTICLE_H
#define PARTICLE_H
#include "utilities.h"

namespace PSim {

/**
 * @class particle
 * @author Sawyer Hopkins
 * @date 06/27/15
 * @file particle.h
 * @brief
 */
class particle {

	/********************************************//**
	 *----------------SYSTEM VARIABLES----------------
	 ************************************************/

private:

	//The particle identifier.
	int name;

	//Contains the current spacial information for x,y,z cordinates.
	type3<double> pos;

	//Contains the previous spacial information for x,y,z cordinates.
	type3<double> pos0;

	//Contains the velocity information for x,y,z cordinates.
	type3<double> vel;

	//Contains the current force information for the x,y,z cordinates.
	type3<double> frc;

	//Contains the previous force information for the x,y,z cordinates.
	type3<double> frc0;

	//Contains the radius and mass of each particle.
	double r,m;

	//Contains the current cell identification.
	type3<int> cll;

	//Coordination number.
	int coorNumber;
	//The particles the base particle interacts with.
	std::vector<particle*> interactions;

public:

	//Header Version.
	static const int version = 1;

	/********************************************//**
	 *--------------SYSTEM CONSTRUCTION---------------
	 ************************************************/

	//Constructor/Destructor
	/**
	 * @brief Creates a new particle with specified name.
	 * @param pid The name of the particle.
	 * @return
	 */
	particle(int pid);
	/**
	 * @brief Removes particle resources from memory.
	 * @return Nothing.
	 */
	~particle();

	/********************************************//**
	 *-----------------SYSTEM GETTERS-----------------
	 ************************************************/

	//Getters for current position.
	/**
	 * @brief Get the X position
	 * @return  exposes private variable x.
	 */
	const double getX() const {
		return pos.x;
	}
	/**
	 * @brief Get the Y position
	 * @return  exposes private variable y.
	 */
	const double getY() const {
		return pos.y;
	}
	/**
	 * @brief Get the Z position
	 * @return  exposes private variable z.
	 */
	const double getZ() const {
		return pos.z;
	}

	//Getters for previous position.

	/**
	 * @brief Get the previous X position
	 * @return  exposes private variable x0.
	 */
	const double getX0() const {
		return pos0.x;
	}
	/**
	 * @brief Get the previous Y position
	 * @return  exposes private variable y0.
	 */
	const double getY0() const {
		return pos0.y;
	}
	/**
	 * @brief Get the previous X position
	 * @return  exposes private variable z0.
	 */
	const double getZ0() const {
		return pos0.z;
	}

	//Getters for velocity.

	/**
	 * @brief Get the x velocity.
	 * @return  exposes private variable x.
	 */
	const double getVX() const {
		return vel.x;
	}
	/**
	 * @brief Get the y velocity.
	 * @return  exposes private variable y.
	 */
	const double getVY() const {
		return vel.y;
	}
	/**
	 * @brief Get the z velocity.
	 * @return  exposes private variable z.
	 */
	const double getVZ() const {
		return vel.z;
	}

	//Getters for current force.

	/**
	 * @brief Get the current x force.
	 * @return  exposes private variable fx.
	 */
	const double getFX() const {
		return frc.x;
	}
	/**
	 * @brief Get the current y force.
	 * @return  exposes private variable fy.
	 */
	const double getFY() const {
		return frc.y;
	}
	/**
	 * @brief Get the current z force.
	 * @return  exposes private variable fz.
	 */
	const double getFZ() const {
		return frc.z;
	}

	//Getters for previous force.

	/**
	 * @brief Get the previous x force.
	 * @return  exposes private variable fx0.
	 */
	const double getFX0() const {
		return frc0.x;
	}
	/**
	 * @brief Get the previous y force.
	 * @return  exposes private variable fy0.
	 */
	const double getFY0() const {
		return frc0.y;
	}
	/**
	 * @brief Get the previous z force.
	 * @return  exposes private variable fz0.
	 */
	const double getFZ0() const {
		return frc0.z;
	}

	//Getters for containing cell.

	/**
	 * @brief Get the x value of the containing cell.
	 * @return  exposes private variable cx.
	 */
	const int getCX() const {
		return cll.x;
	}
	/**
	 * @brief Get the y value of the containing cell.
	 * @return  exposes private variable cy.
	 */
	const int getCY() const {
		return cll.y;
	}
	/**
	 * @brief Get the z value of the containing cell.
	 * @return  exposes private variable cz.
	 */
	const int getCZ() const {
		return cll.z;
	}

	//Getters for particle properties

	/**
	 * @brief Get the radius of the particle.
	 * @return  exposes private variable r.
	 */
	const double getRadius() const {
		return r;
	}
	/**
	 * @brief Get the mass of the particle.
	 * @return  exposes private variable m.
	 */
	const double getMass() const {
		return m;
	}
	/**
	 * @brief Get the name of the particle.
	 * @return  exposes private variable name.
	 */
	const double getName() const {
		return name;
	}
	/**
	 * @brief Returns the coordination number.
	 * @return
	 */
	const int getCoorNumber() const {
		return coorNumber;
	}
	/**
	 * @brief Get the interacting particles.
	 * @return
	 */
	const std::vector<particle*> getInteractions() const {
		return interactions;
	}
	/**
	 * @brief The the beginning iterator of the interating particles.
	 * @return iterator::begin
	 */
	const std::vector<particle*>::iterator getInteractionsBegin() {
		return interactions.begin();
	}
	/**
	 * @brief Gets the end off the vector of interacting particles.
	 * @return iterator::end
	 */
	const std::vector<particle*>::iterator getInteractionsEnd() {
		return interactions.end();
	}

	/********************************************//**
	 *-----------------SYSTEM SETTERS-----------------
	 ************************************************/

	/**
	 * @brief Set the x position of the particle. Handles PBC and updates x0.
	 * @param val The new x position.
	 * @param boxSize The size of the system box.
	 */
	void setX(double val, double boxSize);
	/**
	 * @brief Set the y position of the particle. Handles PBC and updates y0.
	 * @param val The new y position.
	 * @param boxSize The size of the system box.
	 */
	void setY(double val, double boxSize);
	/**
	 * @brief Set the z position of the particle. Handles PBC and updates z0.
	 * @param val The new z position.
	 * @param boxSize The size of the system box.
	 */
	void setZ(double val, double boxSize);
	/**
	 * @brief Set the position of the particle. Handles PBC and updates the previous value.
	 * @param xVal,yVal,zVal The new position.
	 * @param boxSize The size of the system box.
	 */
	void setPos(type3<double>* pos, int boxSize);
	/**
	 * @brief Set the x velocity.
	 * @param val The velocity to set.
	 */
	void setVX(double val) {
		vel.x = val;
	}
	/**
	 * @brief Set the y velocity.
	 * @param val The velocity to set.
	 */
	void setVY(double val) {
		vel.y = val;
	}
	/**
	 * @brief Set the z velocity.
	 * @param val The velocity to set.
	 */
	void setVZ(double val) {
		vel.z = val;
	}
	/**
	 * @brief Adds the the current value of force.
	 * @param xVal,yVal,zVal The values of force to add.
	 * @param pot The potential of the force interaction.
	 * @param p The particle providing the force.
	 */
	void updateForce(type3<double>* frc, particle* p, bool countPair = true);

	/**
	 * @brief Calculates the potential via force integration.
	 */
	float calculatePotential();
	/**
	 * @brief Clears the current force and updates previous force. Resets potential, neighbors, and coordination number.
	 */
	void nextIter();
	/**
	 * @brief Set the location of the containing cell.
	 * @param x,y,z The position variables of the containing cell.
	 */
	void setCell(int x, int y, int z) {
		cll.x = x;
		cll.y = y;
		cll.z = z;
	}
	/**
	 * @brief Sets the radius of the particle.
	 * @param val Radius value.
	 */
	void setRadius(double val) {
		r = val;
	}
	/**
	 * @brief Sets the mass of the particle.
	 * @param val Mass value.
	 */
	void setMass(double val) {
		m = val;
	}
	/**
	 * @brief For setting up interaction table.
	 */
	void addInteraction(particle* p) {
		coorNumber++;
		interactions.push_back(p);
	}
	/********************************************//**
	 *------------------SYSTEM OUTPUT-----------------
	 ************************************************/

	/**
	 * @brief Writes the position of the particle to the console.
	 */
	void writePosition() {
		chatterBox.consoleMessage(tos(pos.x) + ", " + tos(pos.y) + ", " + tos(pos.z));
	}

};

}

#endif // PARTICLE_H

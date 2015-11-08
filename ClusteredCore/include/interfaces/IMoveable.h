#ifndef IMOVEABLE_H_
#define IMOVEABLE_H_

class IMoveable {
public:

	virtual ~IMoveable() {};

	virtual double getX();
	virtual double getY();
	virtual double getZ();

	virtual double getX0();
	virtual double getY0();
	virtual double getZ0();

};

#endif /* IMOVEABLE_H_ */

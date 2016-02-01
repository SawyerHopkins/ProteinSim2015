#ifndef TYPE3
#define TYPE3

namespace PSim {

template <class T>
struct type3 {
	T x;
	T y;
	T z;

	type3() : x(0), y(0), z(0) {}
	type3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

};

}
#endif // TYPE3

#include "point.h"
#include <math.h>



Point operator*(Point a, float f)
{
	return Point(a.x * f, a.y * f, a.z * f);
}

Point operator/(Point a, float f)
{
	return Point(a.x / f, a.y / f, a.z / f);
}

Point operator/(float f, Point a)
{
	return Point(a.x / f, a.y / f, a.z / f);
}

Point operator*(float f, Point a)
{
	return Point(a.x * f, a.y * f, a.z * f);
}

Point operator+(Point a, Point b)
{
	return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator-(Point a, Point b)
{
	return Point(a.x - b.x, a.y - b.y, a.z - a.z);
}



Point& Point::operator += (Point rhs){
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

Point& Point::operator -= (Point rhs){
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

Point& Point::operator *= (float rhs){
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return *this;
}

Point& Point::operator /= (float rhs){
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}



Point cross(Point a, Point b) {
	Point c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = b.x * a.z - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

double dot(Point a, Point b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}




double Point::magSq() {
	return x * x + y * y + z * z;
}

double Point::mag() {
	double t = magSq();
	if (t <= 0.0)
		return 0;
	return sqrt(t);
}

void Point::normalize() {
	double m = mag();
	x /= m;
	y /= m;
	z /= m;
}

double Point::at(int i) {
	if (i == 0) return x;
	if (i == 1) return y;
	if (i == 2) return z;
	return -1;
}
#pragma once

#ifndef __POINT_H_
#define __POINT_H_

class Point 
{
public:
	Point() 
	{
		x = y = z = 0.0;
	}

	Point(double a, double b, double c) {
		x = a; y = b; z = c;
	}

	double magSq();
	double mag();
	void normalize();
	double at(int i);

	Point& operator+=(Point rhs);
	Point& operator-=(Point rhs);
	Point& operator*=(float rhs);
	Point& operator/=(float rhs);

	double x, y, z;

};

Point operator*(Point a, float f);
Point operator/(Point a, float f);
Point operator/(float f, Point p);
Point operator*(float f, Point a);
Point operator+(Point a, Point b);
Point operator-(Point a, Point b);


#endif // !__POINT_H_

#include "Point2.h"

#include "Vector2.h"

Point2::Point2() : X(), Y()
{
}

Point2::Point2(float x, float y) : X(x), Y(y)
{
}

Point2 & Point2::operator+=(const Vector2 & vector)
{
	this->X += vector.X;
	this->Y += vector.Y;
	return *this;
}

Point2 & Point2::operator-=(const Vector2 & vector)
{
	this->X -= vector.X;
	this->Y -= vector.Y;
	return *this;
}

bool operator==(const Point2 & left, const Point2 & right)
{
	return(left.X == right.X) && (left.Y == right.Y);
}

bool operator!=(const Point2 & left, const Point2 & right)
{
	return(left.X != right.X) || (left.Y != right.Y);
}

Point2 operator+(Point2 left, const Vector2 & right)
{
	return left += right;
}

Point2 operator-(Point2 left, const Vector2 & right)
{
	return left -= right;
}

Point2 operator+(const Vector2 & left, Point2 & right)
{
	return right += left;
}

Point2 operator-(const Vector2 & left, Point2 & right)
{
	return right -= left;
}

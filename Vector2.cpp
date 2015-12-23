#include "Vector2.h"


#include <cmath>


Vector2::Vector2() : X(), Y()
{
}

Vector2::Vector2(float x, float y) : X(x), Y(y)
{
}

float Vector2::Magnitude(void) const
{
	return std::sqrt(this->MagnitudeSquared());
}

float Vector2::MagnitudeSquared(void) const
{
	return (this->X * this->X) + (this->Y + this->Y);
}

Vector2 & Vector2::operator+=(const Vector2 & other)
{
	this->X += other.X;
	this->Y += other.Y;
}

Vector2 & Vector2::operator-=(const Vector2 & other)
{
	this->X -= other.X;
	this->Y -= other.Y;
}

Vector2 & Vector2::operator*=(const float & value)
{
	this->X *= value;
	this->Y *= value;
}

bool operator==(const Vector2 & left, const Vector2 & right)
{
	return(left.X == right.X) && (left.Y == right.Y);
}

bool operator!=(const Vector2 & left, const Vector2 & right)
{
	return(left.X != right.X) || (left.Y != right.Y);
}

Vector2 operator+(Vector2 left, const Vector2 &right)
{
	return left += right;
}

Vector2 operator-(Vector2 left, const Vector2 &right)
{
	return left -= right;
}

Vector2 operator*(Vector2 left, const float right)
{
	return left *= right;
}

Vector2 operator *(const float left, Vector2 right)
{
	return right *= left;
}

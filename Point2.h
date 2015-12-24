#pragma once

class Vector2;

class Point2
{
public:
	float X;
	float Y;

public:
	Point2(void);
	Point2(float x, float y);

	inline Point2 & operator +=(const Vector2 & vector);
	inline Point2 & operator -=(const Vector2 & vector);

	inline friend bool operator ==(const Point2 & left, const Point2 & right);
	inline friend bool operator !=(const Point2 & left, const Point2 & right);

	inline friend Point2 operator +(Point2 left, const Vector2 & right);
	inline friend Point2 operator -(Point2 left, const Vector2 & right);
	inline friend Point2 operator +(const Vector2 &  left, Point2 right);
	inline friend Point2 operator -(const Vector2 &  left, Point2 right);
};

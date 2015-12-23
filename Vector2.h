#pragma once

class Vector2
{
public:
	float X;
	float Y;

public:
	Vector2();
	Vector2(float x, float y);

	inline float Magnitude(void) const;
	inline float MagnitudeSquared(void) const;

	inline Vector2 & operator +=(const Vector2 & other);
	inline Vector2 & operator -=(const Vector2 & other);
	inline Vector2 & operator *=(const float & value);

	inline friend bool operator ==(const Vector2 & left, const Vector2 & right);
	inline friend bool operator !=(const Vector2 & left, const Vector2 & right);

	inline friend Vector2 operator +(Vector2 left, const Vector2 &right) { return left += right; }
	inline friend Vector2 operator -(Vector2 left, const Vector2 &right) { return left -= right; }
	inline friend Vector2 operator *(Vector2 left, const float right) { return left *= right; }
	inline friend Vector2 operator *(const float left, Vector2 right) { return right *= left; }
};

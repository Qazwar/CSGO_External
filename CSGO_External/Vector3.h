#pragma once

#include <iostream>
#include <cmath>
#include <assert.h>

#define M_PI       3.14159265358979323846   // pi

#define M_RAD2DEG_CONVERSION ( 180.f / M_PI )
#define M_RAD2DEG(rad) float(rad * M_RAD2DEG_CONVERSION)

#define M_DEG2RAD_CONVERSION ( M_PI / 180.f )
#define M_DEG2RAD(deg) float(deg * M_DEG2RAD_CONVERSION)

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
	return clamp(v, lo, hi, std::less<>());
}
template<class T, class Compare>
constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
{
	return assert(!comp(hi, lo)), comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

typedef struct Vector3_t
{
	/*
	Data poitns
	*/
	float m_flXPos, m_flYPos, m_flZPos;

	/*
	Constructors
	*/

	Vector3_t()
	{
		this->m_flXPos = this->m_flYPos = this->m_flZPos = 0;
	}

	Vector3_t(float scalar)
	{
		this->m_flXPos = scalar;
		this->m_flYPos = scalar;
		this->m_flZPos = scalar;
	}

	Vector3_t(float x, float y, float z)
	{
		this->m_flXPos = x;
		this->m_flYPos = y;
		this->m_flZPos = z;
	}

	/*
	Vector Assignment
	*/

	Vector3_t& operator=(const Vector3_t& rhs)
	{
		this->m_flXPos = rhs.m_flXPos;
		this->m_flYPos = rhs.m_flYPos;
		this->m_flZPos = rhs.m_flZPos;
		return *this;
	}

	Vector3_t& operator+=(const Vector3_t& rhs)
	{
		this->m_flXPos += rhs.m_flXPos;
		this->m_flYPos += rhs.m_flYPos;
		this->m_flZPos += rhs.m_flZPos;
		return *this;
	}

	Vector3_t& operator-=(const Vector3_t& rhs)
	{
		this->m_flXPos -= rhs.m_flXPos;
		this->m_flYPos -= rhs.m_flYPos;
		this->m_flZPos -= rhs.m_flZPos;
		return *this;
	}

	Vector3_t& operator*=(const Vector3_t& rhs)
	{
		this->m_flXPos *= rhs.m_flXPos;
		this->m_flYPos *= rhs.m_flYPos;
		this->m_flZPos *= rhs.m_flZPos;
		return *this;
	}

	Vector3_t& operator/=(const Vector3_t& rhs)
	{
		this->m_flXPos /= rhs.m_flXPos;
		this->m_flYPos /= rhs.m_flYPos;
		this->m_flZPos /= rhs.m_flZPos;
		return *this;
	}

	Vector3_t& operator%=(const Vector3_t& rhs)
	{
		this->m_flXPos = fmod(this->m_flXPos, rhs.m_flXPos);
		this->m_flYPos = fmod(this->m_flYPos, rhs.m_flYPos);
		this->m_flZPos = fmod(this->m_flZPos, rhs.m_flZPos);
		return *this;
	}

	/*
	Vector Arithmetic
	*/

	Vector3_t operator+(const Vector3_t& rhs)
	{
		return Vector3_t(this->m_flXPos + rhs.m_flXPos, this->m_flYPos + rhs.m_flYPos, this->m_flZPos + rhs.m_flZPos);
	}

	Vector3_t operator-(const Vector3_t& rhs)
	{
		return Vector3_t(this->m_flXPos - rhs.m_flXPos, this->m_flYPos - rhs.m_flYPos, this->m_flZPos - rhs.m_flZPos);
	}

	Vector3_t operator*(const Vector3_t& rhs)
	{
		return Vector3_t(this->m_flXPos * rhs.m_flXPos, this->m_flYPos * rhs.m_flYPos, this->m_flZPos * rhs.m_flZPos);
	}

	Vector3_t operator/(const Vector3_t& rhs)
	{
		return Vector3_t(this->m_flXPos / rhs.m_flXPos, this->m_flYPos / rhs.m_flYPos, this->m_flZPos / rhs.m_flZPos);
	}

	Vector3_t operator%(const Vector3_t& rhs)
	{
		return Vector3_t(fmod(this->m_flXPos, rhs.m_flXPos), fmod(this->m_flYPos, rhs.m_flYPos), fmod(this->m_flZPos, rhs.m_flZPos));
	}

	/*
	Vector Comparisons
	*/

	bool operator==(const Vector3_t& rhs)
	{
		return (this->m_flXPos == rhs.m_flXPos && this->m_flYPos == rhs.m_flYPos && this->m_flZPos == rhs.m_flZPos);
	}

	bool operator!=(const Vector3_t& rhs)
	{
		return (this->m_flXPos != rhs.m_flXPos && this->m_flYPos != rhs.m_flYPos && this->m_flZPos != rhs.m_flZPos);
	}

	bool operator>(const Vector3_t& rhs)
	{
		return (this->m_flXPos > rhs.m_flXPos && this->m_flYPos > rhs.m_flYPos && this->m_flZPos > rhs.m_flZPos);
	}

	bool operator<(const Vector3_t& rhs)
	{
		return (this->m_flXPos < rhs.m_flXPos && this->m_flYPos < rhs.m_flYPos && this->m_flZPos < rhs.m_flZPos);
	}

	bool operator>=(const Vector3_t& rhs)
	{
		return (this->m_flXPos >= rhs.m_flXPos && this->m_flYPos >= rhs.m_flYPos && this->m_flZPos >= rhs.m_flZPos);
	}

	bool operator<=(const Vector3_t& rhs)
	{
		return (this->m_flXPos <= rhs.m_flXPos && this->m_flYPos <= rhs.m_flYPos && this->m_flZPos <= rhs.m_flZPos);
	}

	/*
	Other Vector Operations
	*/

	float GetX() { return m_flXPos; };
	float GetY() { return m_flYPos; };
	float GetZ() { return m_flZPos; };
	bool IsValid() { return m_flXPos && m_flYPos; };

	auto IsNull()
	{
		return (m_flXPos <= 0.0 || m_flYPos <= 0.0);
	}

	float Length(float Is3D = true)
	{
		return this->Length3D();
	}

	float Length2D()
	{
		return std::sqrt(std::pow(this->m_flXPos, 2) + std::pow(this->m_flYPos, 2));
	}

	float Length3D()
	{
		return std::sqrt(std::pow(this->m_flXPos, 2) + std::pow(this->m_flYPos, 2) + std::pow(this->m_flZPos, 2));
	}

	float Distance(const Vector3_t& rhs)
	{
		return std::sqrt(std::pow(this->m_flXPos - rhs.m_flXPos, 2) + std::pow(this->m_flYPos - rhs.m_flYPos, 2) + std::pow(this->m_flZPos - rhs.m_flZPos, 2));
	}

	float Dot(const Vector3_t& rhs)
	{
		return (this->m_flXPos * rhs.m_flXPos) + (this->m_flYPos * rhs.m_flYPos) + (this->m_flZPos * rhs.m_flZPos);
	}

	Vector3_t Cross(const Vector3_t& rhs)
	{
		return Vector3_t(
			((this->m_flYPos * rhs.m_flZPos) - (this->m_flZPos * rhs.m_flYPos)),
			((this->m_flZPos * rhs.m_flXPos) - (this->m_flXPos * rhs.m_flZPos)),
			((this->m_flXPos * rhs.m_flYPos) - (this->m_flYPos * rhs.m_flXPos))
		);
	}

	void Normalize()
	{
		float flLength = this->Length();
		if (flLength == 0)
			return;

		this->m_flXPos /= flLength;
		this->m_flYPos /= flLength;
		this->m_flZPos /= flLength;
	}

	Vector3_t ToAngle()
	{
		Vector3_t vAngle(0.f);

		if (this->m_flXPos == 0 && this->m_flYPos == 0)
		{
			vAngle.m_flYPos = 0;
			if (this->m_flZPos > 0)
				vAngle.m_flXPos = 270;
			else
				vAngle.m_flXPos = 90;
		}
		else
		{
			vAngle.m_flYPos = float(M_RAD2DEG(atan2(this->m_flYPos, this->m_flXPos)));
			if (vAngle.m_flYPos < 0)
				vAngle.m_flYPos += 360;

			vAngle.m_flXPos = float(M_RAD2DEG(atan2(-this->m_flZPos, this->Length())));
			if (vAngle.m_flXPos < 0)
				vAngle.m_flXPos += 360;
		}

		vAngle.m_flZPos = 0.f;
		return vAngle;
	}

	Vector3_t ToAngleOld()
	{
		Vector3_t vAngle(0.f);
		vAngle.m_flXPos = (float)(atan(this->m_flZPos / sqrt(this->m_flXPos * this->m_flXPos + this->m_flYPos * this->m_flYPos)) * M_RAD2DEG_CONVERSION);
		vAngle.m_flYPos = (float)(atan(this->m_flYPos / this->m_flXPos) * M_RAD2DEG_CONVERSION);
		vAngle.m_flZPos = 0.f;

		if (this->m_flXPos >= 0.0)
			vAngle.m_flYPos += 180.0f;

		return vAngle;
	}

	Vector3_t ToVector()
	{
		float sinYaw = sin(M_DEG2RAD(this->m_flYPos));
		float cosYaw = cos(M_DEG2RAD(this->m_flYPos));

		float sinPitch = sin(M_DEG2RAD(this->m_flXPos));
		float cosPitch = cos(M_DEG2RAD(this->m_flXPos));

		Vector3_t vVector(0.f);
		vVector.m_flXPos = cosPitch * cosYaw;
		vVector.m_flYPos = cosPitch * sinYaw;
		vVector.m_flZPos = -sinPitch;
		return vVector;
	}

	bool IsNAN()
	{
		return std::isnan(this->m_flXPos) || std::isnan(this->m_flYPos) || std::isnan(this->m_flZPos);
	}

	/*
	Clamp Functions
	*/

	void Clamp(Vector3_t & rhs)
	{
		this->m_flXPos = clamp(std::remainder(rhs.m_flXPos, 180.f), -rhs.m_flXPos, +rhs.m_flXPos);
		this->m_flYPos = clamp(std::remainder(rhs.m_flYPos, 360.f), -rhs.m_flYPos, +rhs.m_flYPos);
		this->m_flZPos = 0.0f;
	}
} Vector3;

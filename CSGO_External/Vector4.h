#pragma once
#include "main.h"

typedef struct Vector4_t
{
	/*
	Data poitns
	*/
	float m_flIPos, m_flJPos, m_flKPos, m_flLPos;

	/*
	Constructors
	*/

	Vector4_t()
	{
		this->m_flIPos = this->m_flJPos = this->m_flKPos = this->m_flLPos = 0;
	}

	Vector4_t(float scalar)
	{
		this->m_flIPos = scalar;
		this->m_flJPos = scalar;
		this->m_flKPos = scalar;
		this->m_flLPos = scalar;
	}

	Vector4_t(float i, float j, float k, float l)
	{
		this->m_flIPos = i;
		this->m_flJPos = j;
		this->m_flKPos = k;
		this->m_flLPos = l;
	}

	/*
	Vector Assignment
	*/

	Vector4_t& operator=(const Vector4_t& rhs)
	{
		this->m_flIPos = rhs.m_flIPos;
		this->m_flJPos = rhs.m_flJPos;
		this->m_flKPos = rhs.m_flKPos;
		this->m_flLPos = rhs.m_flLPos;
		return *this;
	}

	Vector4_t& operator+=(const Vector4_t& rhs)
	{
		this->m_flIPos += rhs.m_flIPos;
		this->m_flJPos += rhs.m_flJPos;
		this->m_flKPos += rhs.m_flKPos;
		this->m_flLPos += rhs.m_flLPos;
		return *this;
	}

	Vector4_t& operator-=(const Vector4_t& rhs)
	{
		this->m_flIPos -= rhs.m_flIPos;
		this->m_flJPos -= rhs.m_flJPos;
		this->m_flKPos -= rhs.m_flKPos;
		this->m_flLPos -= rhs.m_flLPos;
		return *this;
	}

	Vector4_t& operator*=(const Vector4_t& rhs)
	{
		this->m_flIPos *= rhs.m_flIPos;
		this->m_flJPos *= rhs.m_flJPos;
		this->m_flKPos *= rhs.m_flKPos;
		this->m_flLPos *= rhs.m_flLPos;
		return *this;
	}

	Vector4_t& operator/=(const Vector4_t& rhs)
	{
		this->m_flIPos /= rhs.m_flIPos;
		this->m_flJPos /= rhs.m_flJPos;
		this->m_flKPos /= rhs.m_flKPos;
		this->m_flLPos /= rhs.m_flLPos;
		return *this;
	}

	Vector4_t& operator%=(const Vector4_t& rhs)
	{
		this->m_flIPos = fmod(this->m_flIPos, rhs.m_flIPos);
		this->m_flJPos = fmod(this->m_flJPos, rhs.m_flJPos);
		this->m_flKPos = fmod(this->m_flKPos, rhs.m_flKPos);
		this->m_flLPos = fmod(this->m_flLPos, rhs.m_flLPos);
		return *this;
	}

	/*
	Vector Arithmetic
	*/

	Vector4_t operator+(const Vector4_t& rhs)
	{
		return Vector4_t(this->m_flIPos + rhs.m_flIPos, this->m_flJPos + rhs.m_flJPos, this->m_flKPos + rhs.m_flKPos, this->m_flLPos + rhs.m_flLPos);
	}

	Vector4_t operator-(const Vector4_t& rhs)
	{
		return Vector4_t(this->m_flIPos - rhs.m_flIPos, this->m_flJPos - rhs.m_flJPos, this->m_flKPos - rhs.m_flKPos, this->m_flLPos - rhs.m_flLPos);
	}

	Vector4_t operator*(const Vector4_t& rhs)
	{
		return Vector4_t(this->m_flIPos * rhs.m_flIPos, this->m_flJPos * rhs.m_flJPos, this->m_flKPos * rhs.m_flKPos, this->m_flLPos * rhs.m_flLPos);
	}

	Vector4_t operator/(const Vector4_t& rhs)
	{
		return Vector4_t(this->m_flIPos / rhs.m_flIPos, this->m_flJPos / rhs.m_flJPos, this->m_flKPos / rhs.m_flKPos, this->m_flLPos / rhs.m_flLPos);
	}

	Vector4_t operator%(const Vector4_t& rhs)
	{
		return Vector4_t(fmod(this->m_flIPos, rhs.m_flIPos), fmod(this->m_flJPos, rhs.m_flJPos), fmod(this->m_flKPos, rhs.m_flKPos), fmod(this->m_flLPos, rhs.m_flLPos));
	}

	/*
	Vector Comparisons
	*/

	bool operator==(const Vector4_t& rhs)
	{
		return (this->m_flIPos == rhs.m_flIPos && this->m_flJPos == rhs.m_flJPos && this->m_flKPos == rhs.m_flKPos && this->m_flLPos == rhs.m_flLPos);
	}

	bool operator!=(const Vector4_t& rhs)
	{
		return (this->m_flIPos != rhs.m_flIPos && this->m_flJPos != rhs.m_flJPos && this->m_flKPos != rhs.m_flKPos && this->m_flLPos != rhs.m_flLPos);
	}

	bool operator>(const Vector4_t& rhs)
	{
		return (this->m_flIPos > rhs.m_flIPos && this->m_flJPos > rhs.m_flJPos && this->m_flKPos > rhs.m_flKPos && this->m_flLPos > rhs.m_flLPos);
	}

	bool operator<(const Vector4_t& rhs)
	{
		return (this->m_flIPos < rhs.m_flIPos && this->m_flJPos < rhs.m_flJPos && this->m_flKPos < rhs.m_flKPos && this->m_flLPos < rhs.m_flLPos);
	}

	bool operator>=(const Vector4_t& rhs)
	{
		return (this->m_flIPos >= rhs.m_flIPos && this->m_flJPos >= rhs.m_flJPos && this->m_flKPos >= rhs.m_flKPos && this->m_flLPos >= rhs.m_flLPos);
	}

	bool operator<=(const Vector4_t& rhs)
	{
		return (this->m_flIPos <= rhs.m_flIPos && this->m_flJPos <= rhs.m_flJPos && this->m_flKPos <= rhs.m_flKPos && this->m_flLPos <= rhs.m_flLPos);
	}
} Vector4;


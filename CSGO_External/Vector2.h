#pragma once

typedef struct Vector2_t
{
	template<class T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
		return clamp(v, lo, hi, std::less<>());
	}
	template<class T, class Compare>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
	{
		return assert(!comp(hi, lo)),
			comp(v, lo) ? lo : comp(hi, v) ? hi : v;
	}

	/*
	Data poitns
	*/
	float m_flXPos, m_flYPos;

	/*
	Constructors
	*/

	Vector2_t()
	{
		this->m_flXPos = this->m_flYPos = 0;
	}

	Vector2_t(float scalar)
	{
		this->m_flXPos = scalar;
		this->m_flYPos = scalar;
	}

	Vector2_t(float x, float y)
	{
		this->m_flXPos = x;
		this->m_flYPos = y;
	}

	/*
	Vector Assignment
	*/

	Vector2_t& operator=(const Vector2_t& rhs)
	{
		this->m_flXPos = rhs.m_flXPos;
		this->m_flYPos = rhs.m_flYPos;
		return *this;
	}

	Vector2_t& operator+=(const Vector2_t& rhs)
	{
		this->m_flXPos += rhs.m_flXPos;
		this->m_flYPos += rhs.m_flYPos;
		return *this;
	}

	Vector2_t& operator-=(const Vector2_t& rhs)
	{
		this->m_flXPos -= rhs.m_flXPos;
		this->m_flYPos -= rhs.m_flYPos;
		return *this;
	}

	Vector2_t& operator*=(const Vector2_t& rhs)
	{
		this->m_flXPos *= rhs.m_flXPos;
		this->m_flYPos *= rhs.m_flYPos;
		return *this;
	}

	Vector2_t& operator/=(const Vector2_t& rhs)
	{
		this->m_flXPos /= rhs.m_flXPos;
		this->m_flYPos /= rhs.m_flYPos;
		return *this;
	}

	Vector2_t& operator%=(const Vector2_t& rhs)
	{
		this->m_flXPos = fmod(this->m_flXPos, rhs.m_flXPos);
		this->m_flYPos = fmod(this->m_flYPos, rhs.m_flYPos);
		return *this;
	}

	/*
	Vector Arithmetic
	*/

	Vector2_t operator+(const Vector2_t& rhs)
	{
		return Vector2_t(this->m_flXPos + rhs.m_flXPos, this->m_flYPos + rhs.m_flYPos);
	}

	Vector2_t operator-(const Vector2_t& rhs)
	{
		return Vector2_t(this->m_flXPos - rhs.m_flXPos, this->m_flYPos - rhs.m_flYPos);
	}

	Vector2_t operator*(const Vector2_t& rhs)
	{
		return Vector2_t(this->m_flXPos * rhs.m_flXPos, this->m_flYPos * rhs.m_flYPos);
	}

	Vector2_t operator/(const Vector2_t& rhs)
	{
		return Vector2_t(this->m_flXPos / rhs.m_flXPos, this->m_flYPos / rhs.m_flYPos);
	}

	Vector2_t operator%(const Vector2_t& rhs)
	{
		return Vector2_t(fmod(this->m_flXPos, rhs.m_flXPos), fmod(this->m_flYPos, rhs.m_flYPos));
	}

	/*
	Vector Comparisons
	*/

	bool operator==(const Vector2_t& rhs)
	{
		return (this->m_flXPos == rhs.m_flXPos && this->m_flYPos == rhs.m_flYPos);
	}

	bool operator!=(const Vector2_t& rhs)
	{
		return (this->m_flXPos != rhs.m_flXPos && this->m_flYPos != rhs.m_flYPos);
	}

	bool operator>(const Vector2_t& rhs)
	{
		return (this->m_flXPos > rhs.m_flXPos && this->m_flYPos > rhs.m_flYPos);
	}

	bool operator<(const Vector2_t& rhs)
	{
		return (this->m_flXPos < rhs.m_flXPos && this->m_flYPos < rhs.m_flYPos);
	}

	bool operator>=(const Vector2_t& rhs)
	{
		return (this->m_flXPos >= rhs.m_flXPos && this->m_flYPos >= rhs.m_flYPos);
	}

	bool operator<=(const Vector2_t& rhs)
	{
		return (this->m_flXPos <= rhs.m_flXPos && this->m_flYPos <= rhs.m_flYPos);
	}

	/*
	Other Vector Operations
	*/

	float GetX() { return m_flXPos; };
	float GetY() { return m_flYPos; };
	bool IsValid() { return m_flXPos && m_flYPos; };

	float Length()
	{
		return std::sqrt(std::pow(this->m_flXPos, 2) + std::pow(this->m_flYPos, 2));
	}

	float Distance(const Vector2_t& rhs)
	{
		return std::sqrt(std::pow(this->m_flXPos - rhs.m_flXPos, 2) + std::pow(this->m_flYPos - rhs.m_flYPos, 2));
	}

	void Normalize()
	{
		float flLength = this->Length();
		if (flLength == 0)
			return;

		this->m_flXPos /= flLength;
		this->m_flYPos /= flLength;
	}

	bool IsNAN()
	{
		return std::isnan(this->m_flXPos) || std::isnan(this->m_flYPos);
	}

	/*
	Clamp Functions
	*/

	void Clamp(Vector2_t & rhs)
	{
		this->m_flXPos = clamp(std::remainder(rhs.m_flXPos, 180.f), -rhs.m_flXPos, +rhs.m_flXPos);
		this->m_flYPos = clamp(std::remainder(rhs.m_flYPos, 360.f), -rhs.m_flYPos, +rhs.m_flYPos);
	}
} Vector2;

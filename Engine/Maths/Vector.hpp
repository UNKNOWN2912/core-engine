#pragma once
#include <cstddef>
#include <cstdint>

using byte = unsigned char;

template<typename T>
struct Vector2
{
	union
	{
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
	};

	Vector2() : x(0), y(0) {}

	Vector2(T x, T y): x(x), y(y) { }

	Vector2(const Vector2& value)
	{
		x = value.x;
		y = value.y;
	}

	void operator=(const Vector2& value) { x = value.x; y = value.y; }

	Vector2 operator +(const Vector2& value) const
	{
		return Vector2(x + value.x, y + value.y);
	}
	Vector2 operator -(const Vector2& value)  const
	{
		return Vector2(x - value.x, y - value.y);
	}
	Vector2 operator *(const Vector2& value) const
	{
		return Vector2(x * value.x, y * value.y);
	}
	Vector2 operator /(const Vector2& value) const
	{
		return Vector2(x / value.x, y / value.y);
	}

	Vector2& operator += (const Vector2 & value)
	{
		x += value.x;
		y += value.y;
		return *this;
	}
	Vector2& operator -= (const Vector2 & value)
	{
		x -= value.x;
		y -= value.y;
		return *this;
	}
	Vector2& operator *= (const Vector2 & value)
	{
		x *= value.x;
		y *= value.y;
		return *this;
	}
	Vector2& operator /= (const Vector2 & value)
	{
		x /= value.x;
		y /= value.y;
		return *this;
	}

	Vector2<T> operator *(const T& value) const
	{
		return Vector2<T>(x * value, y * value);
	}
};


template<typename T>
struct Vector3
{
	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { Vector2<T> rg; };
	};


	Vector3() : x(0), y(0), z(0) {}

	Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

	Vector3(const Vector3& value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
	}

	void operator=(const Vector3& value) { x = value.x; y = value.y; z = z.value; }


	Vector3 operator +(const Vector3& value) const
	{
		return Vector3(x + value.x, y + value.y, z + value.z);
	}
	Vector3 operator -(const Vector3& value) const
	{
		return Vector3(x - value.x, y - value.y, z - value.z);
	}
	Vector3 operator *(const Vector3& value) const
	{
		return Vector3(x * value.x, y * value.y, z * value.z);
	}
	Vector3 operator /(const Vector3& value) const
	{
		return Vector3(x / value.x, y / value.y, z / value.z);
	}

	Vector3& operator += (const Vector3 & value)
	{
		x += value.x;
		y += value.y;
		z += value.z;
		return *this;
	}
	Vector3& operator -= (const Vector3 & value)
	{
		x -= value.x;
		y -= value.y;
		z -= value.z;
		return *this;
	}
	Vector3& operator *= (const Vector3 & value)
	{
		x *= value.x;
		y *= value.y;
		z *= value.z;
		return *this;
	}
	Vector3& operator /= (const Vector3 & value)
	{
		x /= value.x;
		y /= value.y;
		z /= value.z;
		return *this;
	}

	Vector3<T> operator *(const T& value) const
	{
		return Vector3<T>(x * value, y * value, z * value);
	}

	operator Vector2<T>() const
	{
		return Vector2(x, y);
	}
};


template<typename T>
struct Vector4
{
	union
	{
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		struct { Vector3<T> rgb; };
		struct { Vector2<T> rg; };
	};

	Vector4() : x(0), y(0), z(0), w(0) {}

	Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

	Vector4(const Vector4& value)
	{
		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
	}

	void operator=(const Vector4& value) { x = value.x; y = value.y; z = z.value; w = value.w; }

	Vector4 operator +(const Vector4& value) const
	{
		return Vector4(x + value.x, y + value.y, z + value.z, w + value.w);
	}
	Vector4 operator -(const Vector4& value) const
	{
		return Vector4(x - value.x, y - value.y, z - value.z, w - value.w);
	}
	Vector4 operator *(const Vector4& value) const
	{
		return Vector4(x * value.x, y * value.y, z * value.z, w * value.w);
	}
	Vector4 operator /(const Vector4& value) const
	{
		return Vector4(x / value.x, y / value.y, z / value.z, w / value.w);
	}

	Vector4& operator += (const Vector4 & value)
	{
		x += value.x;
		y += value.y;
		z += value.z;
		w += value.w;
		return *this;
	}
	Vector4& operator -= (const Vector4 & value)
	{
		x -= value.x;
		y -= value.y;
		z -= value.z;
		w -= value.w;
		return *this;
	}
	Vector4& operator *= (const Vector4 & value)
	{
		x *= value.x;
		y *= value.y;
		z *= value.z;
		w *= value.w;
		return *this;
	}
	Vector4& operator /=(const Vector4& value)
	{
		x /= value.x;
		y /= value.y;
		z /= value.z;
		w /= value.w;
		return *this;
	}
	 
	Vector4<T> operator *(const T& value) const
	{
		return Vector4<T>(x * value, y * value, z * value, w * value);
	}

	operator Vector2<T>() const
	{
		return Vector2(x, y);
	}

	operator Vector3<T>() const
	{
		return Vector3(x, y, z);
	}
};


using Vector2f = Vector2<float>; 
using Vector3f = Vector3<float>; 
using Vector4f = Vector4<float>;

using Vector2d = Vector2<double>;
using Vector3d = Vector3<double>;
using Vector4d = Vector4<double>;

using Vector2i = Vector2<int32_t>;
using Vector3i = Vector3<int32_t>;
using Vector4i = Vector4<int32_t>;

using Vector2u = Vector2<uint32_t>;
using Vector3u = Vector3<uint32_t>;
using Vector4u = Vector4<uint32_t>;

using Vector2b = Vector2<byte>;
using Vector3b = Vector3<byte>;
using Vector4b = Vector4<byte>;

#ifndef DATATYPES_HPP
#define DATATYPES_HPP

typedef uint8_t connection_t;

const connection_t FULLY_CONNECTED = 63;

const int EMPTY = 0;
const int PARTIALLY_FILLED = 1;
const int FILLED = 2;

template <typename T>
long long isq(T a)
{
	return (long long(a)) * (long long(a));
}
struct vec
{
	int x, y, z;

	vec()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	vec(int x, int y, int z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	int& operator[](int i)
	{
		assert(("Unallowed access to index in vec structure", i >= 0 && i < 3));

		if (i == 0)
			return x;
		if (i == 1)
			return y;
		if (i == 2)
			return z;

		return x;
	}

	bool operator==(vec other)
	{
		return (x == other.x) && (y == other.y) && (z == other.z);
	}

	bool operator!=(vec other)
	{
		return !operator==(other);
	}

	vec operator+(int other)
	{
		return vec(x + other, y + other, z + other);
	}

	vec operator+(vec other)
	{
		return vec(x + other.x, y + other.y, z + other.z);
	}

	vec operator-(vec other)
	{
		return vec(x - other.x, y - other.y, z - other.z);
	}

	bool operator<(vec other)
	{
		return (x < other.x) && (y < other.y) && (z < other.z);
	}

	bool operator<=(vec other)
	{
		return (x <= other.x) && (y <= other.y) && (z <= other.z);
	}

	bool operator>(vec other)
	{
		return (x > other.x) && (y > other.y) && (z > other.z);
	}

	bool operator>=(vec other)
	{
		return (x >= other.x) && (y >= other.y) && (z >= other.z);
	}

	int size()
	{
		return 3;
	}

	float length()
	{
		return sqrt((long long)x * (long long)x + (long long)y * (long long)y + (long long)z * (long long)z);
	}

	long long sqlength()
	{
		return (long long)x * (long long)x + (long long)y * (long long)y + (long long)z * (long long)z;
	}
};

struct tri
{
public:
	vec a, b, c;

	tri(vec a, vec b, vec c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
	}

	vec& operator[](int i)
	{
		assert(("Unallowed access to index in triangle structure", i >= 0 && i < 3));

		if (i == 0)
			return a;
		if (i == 1)
			return b;
		if (i == 2)
			return c;
		
		return a;
	}

	int size()
	{
		return 3;
	}

	tri() {};
};

struct rgb
{
	float r, g, b;

	rgb()
	{
		r = 0;
		g = 0;
		b = 0;
	}

	rgb(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	float& operator[](int i)
	{
		assert(("Unallowed access to index in rgb structure", i >= 0 && i < 3));

		if (i == 0)
			return r;
		if (i == 1)
			return g;
		if (i == 2)
			return b;
		
		return r;
	}

	int size()
	{
		return 3;
	}
};

#endif

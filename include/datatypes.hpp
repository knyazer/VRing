#ifndef DATATYPES_HPP
#define DATATYPES_HPP

struct vec
{
	float x, y, z;

	vec()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	vec(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float& operator[](int i)
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

	int size()
	{
		return 3;
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

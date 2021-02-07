#ifndef VOXEL_HPP
#define VOXEL_HPP

rgb COLORS[6] = { rgb(0.f, 0.5f, 0.3f), rgb(0.f, 1.f, 0.f), rgb(0.f, 0.f, 1.f), rgb(1.f, 0.f, 0.5f), rgb(0.3f, 0.2f, 0.5f), rgb(0.8f, 0.8f, 0.8f) };

class Voxel
{
public:
	vec pos;
	int size;

	vector<rgb> colors;
	vector<tri> faces;

	Voxel() 
	{
		size = 0;
	}

	Voxel(vec pos, float size)
	{
		this->pos = pos;
		this->size = size;
	}

	void setPosition(vec pos)
	{
		this->pos = pos;
	}

	void setSize(float size)
	{
		this->size = size;
	}

	void calculateTri()
	{
		vec a, b, c, d, e, f, g, h;
		a.x = b.x = e.x = f.x = pos.x;
		c.x = d.x = g.x = h.x = pos.x + size;
		a.y = d.y = e.y = h.y = pos.y + size;
		b.y = c.y = f.y = g.y = pos.y;
		a.z = b.z = c.z = d.z = pos.z;
		e.z = f.z = g.z = h.z = pos.z + size;

		faces =
		{
			tri(b, a, c),
			tri(d, a, c),
			tri(a, b, e),
			tri(f, b, e),
			tri(b, f, c),
			tri(g, f, c),
			tri(c, g, d),
			tri(h, g, d),
			tri(e, f, h),
			tri(g, f, h),
			tri(d, a, h),
			tri(e, a, h)
		};

		colors = 
		{
			rgb(1, 0, 0), 
			rgb(0, 0, 1), 
			rgb(0, 1, 0), 
			rgb(1, 1, 0), 
			rgb(0, 1, 1), 
			rgb(1, 0, 1)
		};
	}

	void update(vector<float> &vertexes, vector<float> &colors, vector<float> &barycentrics)
	{
		calculateTri();
		
		updateVertexes(vertexes);
		updateColors(colors);
		updateBarycentrics(barycentrics);
	}

	void updateVertexes(vector<float>& vertexes)
	{
		for (int i = 0; i < 12; i++)
			for (int point = 0; point < 3; point++)
				for (int axis = 0; axis < 3; axis++)
					vertexes.push_back(faces[i][point][axis]);
	}

	void updateColors(vector<float>& colors)
	{
		for (int i = 0; i < 12; i++)
			for (int point = 0; point < 3; point++)
				for (int axis = 0; axis < 3; axis++)
					colors.push_back(this->colors[i >> 1][axis]);
	}

	void updateBarycentrics(vector<float>& barycentrics)
	{
		for (int i = 0; i < 12; i++)
			for (int point = 0; point < 3; point++)
				for (int axis = 0; axis < 3; axis++)
					barycentrics.push_back(point == axis);
	}
};

#endif
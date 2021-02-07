#ifndef VOXEL_HPP
#define VOXEL_HPP

rgb COLORS[6] = { rgb(0.f, 0.5f, 0.3f), rgb(0.f, 1.f, 0.f), rgb(0.f, 0.f, 1.f), rgb(1.f, 0.f, 0.5f), rgb(0.3f, 0.2f, 0.5f), rgb(0.8f, 0.8f, 0.8f) };

class Voxel
{
public:
	vec pos;
	float size;

	vector<rgb> colors;
	vector<tri> faces;

	Voxel() 
	{
		size = 0;
	};

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
		a.x = b.x = e.x = f.x = pos.x - size / 2;
		c.x = d.x = g.x = h.x = pos.x + size / 2;
		a.y = d.y = e.y = h.y = pos.y + size / 2;
		b.y = c.y = f.y = g.y = pos.y - size / 2;
		a.z = b.z = c.z = d.z = pos.z - size / 2;
		e.z = f.z = g.z = h.z = pos.z + size / 2;

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
		for (auto &face : faces)
			for (int point = 0; point < face.size(); point++)
				for (int axis = 0; axis < face[point].size(); axis++)
					vertexes.push_back(face[point][axis]);

	}

	void updateColors(vector<float>& colors)
	{
		int i = 0;

		for (auto &face : faces)
			for (int point = 0; point < face.size(); point++)
				for (int axis = 0; axis < face[point].size(); axis++)
					colors.push_back(this->colors[((i++) / 18)%6][axis]);
	}

	void updateBarycentrics(vector<float>& barycentrics)
	{
		for (auto &face : faces)
			for (int point = 0; point < face.size(); point++)
				for (int axis = 0; axis < face[point].size(); axis++)
					barycentrics.push_back(point == axis);
	}
};

#endif
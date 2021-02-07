#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <include/voxel.hpp>

class Buffer
{
public:
	vector<Voxel>* data;

	void update()
	{
		barycentrics.clear();
		vertexes.clear();
		colors.clear();

		vertexes.reserve(data->size() * 108);
		colors.reserve(data->size() * 108);
		barycentrics.reserve(data->size() * 108);

		for (auto &voxel : *data)
		{
			voxel.update(vertexes, colors, barycentrics);
		}
	}

	vector<float> barycentrics, vertexes, colors;
};

#endif
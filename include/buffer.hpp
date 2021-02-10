#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <include/voxel.hpp>
#include <include/octotree.hpp>
#include <include/camera.hpp>
#include <include/timer.hpp>

class Buffer
{
public:
	Octo* octo;
	Camera* camera;

	Buffer()
	{

	}

	void update()
	{
		Timer t;
		t.begin();

		barycentrics.clear();
		vertexes.clear();
		colors.clear();

		barycentrics.reserve(long(1e7));
		vertexes.reserve(long(1e7));
		colors.reserve(long(1e7));

		updateOcto(octo);
		cout << "Updating octo in buffer:" << t.ms() << endl;
	}

	void updateOcto(Octo* tree)
	{
		// Exit if node is fully empty or fully filled
		if (tree->type == EMPTY || tree->type == FILLED)
			return;

		if (tree->children.size() == 0)
		{
			// Update only boundary voxels
			tree->voxel.update(vertexes, colors, barycentrics);
			return;
		}

		for (Octo& leaf : tree->children)
			updateOcto(&leaf);
	}

	vector<float> barycentrics, vertexes, colors;
};

#endif
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <include/voxel.hpp>
#include <include/octree.hpp>
#include <include/camera.hpp>
#include <include/timer.hpp>

class Buffer
{
public:
	Octree* oct;
	Camera* camera;

	Buffer()
	{

	}

	void update()
	{
		Timer t;
		t.begin();

		vertexes.clear();
		vertexes.reserve(long(1e7));

		connections.clear();
		connections.reserve(long(1e7));

		loadOctree(&oct->root);
		cout << "Updating octo in buffer:" << t.ms() << endl;
	}

	void loadOctree(OctoNode* node)
	{
		// Exit if node is fully empty or fully filled
		if (node->type == EMPTY || node->voxel.connection == FULLY_CONNECTED)
			return;

		if (node->voxel.size == 1)
		{
			// Update only boundary voxels
			node->voxel.update(vertexes, connections);
			return;
		}

		for (OctoNode& leaf : node->children)
			loadOctree(&leaf);
	}

	vector<int> vertexes;
	vector<connection_t> connections;
};

#endif
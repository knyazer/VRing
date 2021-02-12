#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <include/voxel.hpp>
#include <include/octree.hpp>
#include <include/camera.hpp>
#include <include/timer.hpp>
int cnt = 0;
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

		loadOctree(&oct->root);
		cout << "Updating octo in buffer:" << t.ms() << endl;
		cout << cnt << " voxels" << endl;
	}

	void loadOctree(OctoNode* node)
	{
		// Exit if node is fully empty or fully filled
		if (node->type == EMPTY || node->type == FILLED)
			return;

		if (node->children.size() == 0)
		{
			// Update only boundary voxels
			node->voxel.update(vertexes);
			cnt++;
			return;
		}

		for (OctoNode& leaf : node->children)
			loadOctree(&leaf);
	}

	vector<float> vertexes;
};

#endif
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <include/voxel.hpp>
#include <include/octree.hpp>
#include <include/camera.hpp>
#include <include/timer.hpp>

#include <iostream>
#include <algorithm>
using namespace std;

class Buffer
{
public:
	Octree* oct;
	Camera* camera;
	long long voxelsDrawed;
	long long iters;

	Buffer()
	{

	}

	void update()
	{
		Timer t;
		t.begin();

		vertexes.clear();
		vertexes.reserve(long(5e7));

		connections.clear();
		connections.reserve(long(5e7));

		voxelsDrawed = 0;
		iters = 0;
		loadOctree(&oct->root);
		cout << "Updating octo in buffer:" << t.ms() << endl;
		cout << "Voxels drawed: " << voxelsDrawed << endl;
		cout << "Iterations: " << iters << endl;
	}

	void loadOctree(OctoNode* root)
	{
		queue<OctoNode*> q;
		q.push(root);

		while (!q.empty())
		{
			OctoNode* child = &q.front()->children[0];
			q.pop();

			for (int i = 0; i < 8; i++, child++)
			{
				if (child->type == EMPTY || child->voxel.connection == FULLY_CONNECTED)
					continue;
				
				if (child->voxel.size == 1)
				{
					child->voxel.update(vertexes, connections);
					voxelsDrawed++;	
					continue;
				}

				q.push(child);
			}
		}
	}

	void loadOctreeRec(OctoNode* node)
	{
		iters++;
		
		OctoNode* child = &node->children[0];
		for (int i = 0; i < 8; i++, child++)
		{
			if (child->type == EMPTY || child->voxel.connection == FULLY_CONNECTED)
				continue;
			
			if (child->voxel.size == 1)
			{
				child->voxel.update(vertexes, connections);
				voxelsDrawed++;	
				continue;
			}

			loadOctreeRec(child);
		}
	}

	vector<int> vertexes;
	vector<connection_t> connections;
};

#endif
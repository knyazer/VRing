#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <include/voxel.hpp>
#include <include/octree.hpp>
#include <include/camera.hpp>
#include <include/timer.hpp>
#include <include/VList.hpp>

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
					voxelsDrawed++;	

					for (int i = 0; i < 6; i++)
					{
						if ((child->voxel.connection & (1 << i)) == 0)
						{
							g_vlist.add(new Cell());
							(*g_vlist.end).put(child->voxel.pos, i);
						}
					}

					continue;
				}

				q.push(child);
			}
		}
	}
};

#endif
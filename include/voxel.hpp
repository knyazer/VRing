#ifndef VOXEL_HPP
#define VOXEL_HPP

#include <include/datatypes.hpp>
#include <include/camera.hpp>

class Voxel
{
public:
	vec pos;
	int size;
	
    connection_t connection = NO_CONNECTIONS;

	Voxel() 
	{
		size = 0;
	}

	Voxel(vec pos, int size)
	{
		this->pos = pos;
		this->size = size;
	}

	void setPosition(vec pos)
	{
		this->pos = pos;
	}

	void setSize(int size)
	{
		this->size = size;
	}

	void update(vector<int> &vertexes, vector<connection_t> &connections)
	{
		for (int i = 0; i < 3; i++)
			vertexes.push_back(pos[i]);
		
		connections.push_back(connection);
	}
};

#endif
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <include/datatypes.hpp>

class Camera
{
public:
	Camera()
	{
		FOV = radians(60.0f);

		pos = vec3(1000, 1000, 1000);
		COV = vec3(0, 0, 0);
		up = vec3(0, 1, 0);

		updateMVP();
	}

	void updateMVP()
	{
		View = lookAt(
			pos,
			COV,
			up
		);

		Projection = perspective(FOV, 4.0f / 3.0f, 0.1f, 10000.0f);
		MVP = Projection * View;
	}

	void setPosition(vec pos)
	{
		this->pos.r = pos.x;
		this->pos.g = pos.y;
		this->pos.b = pos.z;
	}

	void setPosition(vec3 pos)
	{
		this->pos = pos;
	}

	void setCOV(vec COV)
	{
		this->COV.r = COV.x;
		this->COV.g = COV.y;
		this->COV.b = COV.z;
	}

	void setCOV(vec3 COV)
	{
		this->COV = COV;
	}

	void setUp(vec3 up)
	{
		this->up = up;
	}

	float FOV;
	mat4 Projection, View, MVP;
	vec3 COV, pos, up;
};

#endif
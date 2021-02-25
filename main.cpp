#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
using namespace std;

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <cassert>

#include <include/camera.hpp>
#include <include/gl.hpp>
#include <include/datatypes.hpp>
#include <include/buffer.hpp>
#include <include/voxel.hpp>
#include <include/octree.hpp>
#include <include/timer.hpp>

GL gl;

double verticalAngle = -0.61, horizontalAngle = -2.36;
void computeMatricesFromInputs(Camera &camera) {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(gl.window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(gl.window, 1024 / 2, 768 / 2);

	// Compute new orientation
	horizontalAngle += 0.005 * float(1024 / 2 - xpos);
	verticalAngle += 0.005 * float(768 / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);
	vec3 position = camera.pos;
	// Move forward
	if (glfwGetKey(gl.window, GLFW_KEY_UP) == GLFW_PRESS) {
		position += direction * deltaTime * 50.f;
	}
	// Move backward
	if (glfwGetKey(gl.window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position -= direction * deltaTime * 50.f;
	}
	// Strafe right
	if (glfwGetKey(gl.window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position += right * deltaTime * 50.f;
	}
	// Strafe left
	if (glfwGetKey(gl.window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position -= right * deltaTime * 50.f;
	}

	camera.setPosition(position);
	camera.setCOV(position + direction);
	camera.setUp(up);
	camera.updateMVP();

	lastTime = currentTime;
}

int main(void)
{
	if (!gl.init())
		return -1;

	Camera camera;
	glfwSetCursorPos(gl.window, 1024 / 2, 768 / 2);

	Octree oct(vec(0,0,0), 30);

	Timer t;
	t.begin();

	Sphere sphere(vec(0, 0, 0), 400);
	sphere.put(oct);

	cout << "Building tree: " << t.ms() << endl;

	t.clear();
	Buffer buf;
	buf.oct = &oct;
	buf.camera = &camera;
	gl.bindBuffer(buf);

	cout << "Binding buffer: " << t.ms() << endl;

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	double lastTime = glfwGetTime();
 	int nbFrames = 0;
 	do{
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0)
		{
			printf("%.2f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		computeMatricesFromInputs(camera);
		gl.clear();
		gl.draw(camera);
	}

	while( glfwGetKey(gl.window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(gl.window) == 0 );

	gl.cleanup();

	return 0;
}


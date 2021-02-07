#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
using namespace std;

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <shaders/shader.hpp>
#include <cassert>

struct vec
{
	float x, y, z;

	vec()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	vec(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float& operator[](int i)
	{
		assert(i >= 0 && i < 3);
		if (i == 0)
			return x;
		if (i == 1)
			return y;
		if (i == 2)
			return z;
	}

	int size()
	{
		return 3;
	}
};

struct tri
{
public:
	vec a, b, c;

	tri(vec a, vec b, vec c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
	}

	vec& operator[](int i)
	{
		assert(i >= 0 && i < 3);
		if (i == 0)
			return a;
		if (i == 1)
			return b;
		if (i == 2)
			return c;
	}

	int size()
	{
		return 3;
	}

	tri() {};
};


struct VectorHasher
{
	size_t operator()(vector<tri> const& vec) const {
		size_t seed = vec.size();
		for (auto i : vec) {
			seed ^= size_t(i.a.x * 148738 + 982381) ^ size_t(i.a.y * 12743981 + 17171717) ^ size_t(i.a.z * 183479 + 12374879) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= size_t(i.b.x * 18379 + 982381) ^ size_t(i.b.y * 823425 + 17171717) ^ size_t(i.b.z * 17368 + 12374879) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= size_t(i.c.x * 17346 + 982381) ^ size_t(i.c.y * 1384798 + 17171717) ^ size_t(i.c.z * 7163876 + 12374889) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};


struct rgb
{
	float r, g, b;

	rgb()
	{
		r = 0;
		g = 0;
		b = 0;
	}

	rgb(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	float& operator[](int i)
	{
		assert(i >= 0 && i < 3);
		if (i == 0)
			return r;
		if (i == 1)
			return g;
		if (i == 2)
			return b;
	}

	int size()
	{
		return 3;
	}
};

rgb COLORS[6] = { rgb(0., 0.5, 0.3), rgb(0, 1, 0), rgb(0, 0, 1), rgb(1, 0, 0.5), rgb(0.3, 0.2, 0.5), rgb(0.8, 0.8, 0.8) };

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

class Camera
{
public:
	Camera()
	{
		FOV = radians(60.0f);

		pos = vec3(7, 7, 7);
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

		Projection = perspective(FOV, 4.0f / 3.0f, 0.1f, 1000.0f);
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

class GL
{
public:
	bool initializeGLFW()
	{
		if (!glfwInit())
		{
			fprintf(stderr, "Failed to initialize GLFW\n");
			getchar();
			return false;
		}

		return true;
	}

	void enableAntialiasing()
	{
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	void setVersion()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	}

	void setProfile()
	{
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	void enableMacOS()
	{
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}

	bool initializeWindow()
	{
		window = glfwCreateWindow(1024, 768, "VRing", NULL, NULL);
		if (window == NULL) 
		{
			fprintf(stderr, "Failed to open GLFW window. No OpenGL3 compatible GPU\n");
			getchar();
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);

		return true;
	}

	bool intializeGLEW()
	{
		glewExperimental = true; // Needed for core profile
		if (glewInit() != GLEW_OK) 
		{
			fprintf(stderr, "Failed to initialize GLEW\n");
			getchar();
			glfwTerminate();
			return false;
		}

		return true;
	}

	void enableKeysCapturing()
	{
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	}

	void enableZBuffer()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	void makeVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
	}

	void compileShaders()
	{
		programID = LoadShaders("shaders/TransformVertexShader.vertexshader", "shaders/ColorFragmentShader.fragmentshader");
	}

	void makeMVP()
	{
		matrixID = glGetUniformLocation(programID, "MVP");
	}

	void cameraPart()
	{
		// Camera matrix
		// Our ModelViewProjection : multiplication of our 3 matrices
	}

	void makeVertexBuffer()
	{
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	}

	void makeColorBuffer()
	{
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	}

	void makeBarycentricBuffer()
	{
		glGenBuffers(1, &barycentricBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, barycentricBuffer);
	}

	void bindBuffer(Buffer& buf)
	{
		buf.update();

		makeVertexBuffer();
		glBufferData(GL_ARRAY_BUFFER, buf.vertexes.size() * sizeof(GLfloat), &buf.vertexes[0], GL_DYNAMIC_DRAW);

		makeColorBuffer();
		glBufferData(GL_ARRAY_BUFFER, buf.colors.size() * sizeof(GLfloat), &buf.colors[0], GL_DYNAMIC_DRAW);

		makeBarycentricBuffer();
		glBufferData(GL_ARRAY_BUFFER, buf.barycentrics.size() * sizeof(GLfloat), &buf.barycentrics[0], GL_DYNAMIC_DRAW);

		assert(buf.colors.size() == buf.vertexes.size());

		trianglesCount = buf.vertexes.size();
	}

	void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void enableAttribArrays()
	{
		for (auto ID : attribIDs)
			glEnableVertexAttribArray(ID);
	}

	void disableAttribArrays()
	{
		for (auto ID : attribIDs)
			glDisableVertexAttribArray(ID);
	}

	void draw(Camera &camera)
	{
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &camera.MVP[0][0]);

		enableAttribArrays();
		// 1rst attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3d attribute buffer : barycentric
		glBindBuffer(GL_ARRAY_BUFFER, barycentricBuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, trianglesCount);

		disableAttribArrays();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void cleanup()
	{
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &colorBuffer);
		glDeleteProgram(programID);
		glDeleteVertexArrays(1, &VertexArrayID);

		// Close OpenGL window and terminate GLFW
		glfwTerminate();
	}

	bool init()
	{
		if (!initializeGLFW())
			return 0;

		enableAntialiasing();
		setVersion();
		setProfile();
		enableMacOS();

		if (!initializeWindow())
			return 0;

		if (!intializeGLEW())
			return 0;

		enableKeysCapturing();
		enableZBuffer();
		makeVertexArray();
		compileShaders();
		makeMVP();
		cameraPart();

		return 1;
	}

	GLuint VertexArrayID, programID, matrixID;
	GLuint vertexBuffer, colorBuffer, barycentricBuffer;
	int trianglesCount;

	vector<int> attribIDs = { 0, 1, 2 };
};

GL gl;

double verticalAngle = 30, horizontalAngle = 90;
void computeMatricesFromInputs(Camera &camera) {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

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
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		position += direction * deltaTime * 1.1f;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position -= direction * deltaTime * 1.1f;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position += right * deltaTime * 1.1f;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position -= right * deltaTime * 1.1f;
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

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	vector<Voxel> voxels;
	for (float x = -3; x <= 3; x += 0.3)
		for (float z = -3; z <= 3; z += 0.3)
			voxels.push_back(Voxel(vec(x, 0, z), 0.3));
	
	cout << "Voxels created" << endl;

	Buffer buf;
	buf.data = &voxels;

	gl.bindBuffer(buf);

	cout << "Buffers binded" << endl;

	Camera camera;

	do {
		computeMatricesFromInputs(camera);
		gl.clear();
		gl.draw(camera);
	}

	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	gl.cleanup();

	return 0;
}


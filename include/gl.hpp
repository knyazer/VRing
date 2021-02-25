#ifndef GL_HPP
#define GL_HPP

#include <include/buffer.hpp>
#include <include/shader.hpp>

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
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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
		programID = LoadShaders("shaders/main.vs", "shaders/main.fs", "shaders/main.gs");
	}

	void makeMVP()
	{
		matrixID = glGetUniformLocation(programID, "MVP");
		camPosID = glGetUniformLocation(programID, "camPos");
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

	void makeConnectionBuffer()
	{
		glGenBuffers(1, &connectionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, connectionBuffer);
	}

	void bindBuffer(Buffer& buf)
	{
		buf.update();

		if (buf.vertexes.size() != 0)
		{
			makeVertexBuffer();
			glBufferData(GL_ARRAY_BUFFER, buf.vertexes.size() * sizeof(GLint), &buf.vertexes[0], GL_DYNAMIC_DRAW);

			makeConnectionBuffer();
			glBufferData(GL_ARRAY_BUFFER, buf.connections.size() * sizeof(connection_t), &buf.connections[0], GL_DYNAMIC_DRAW);
		}

		pointsCount = buf.vertexes.size() / 3;
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
		glUniform3fv(camPosID, 1, &extract(camera.pos)[0]);

		enableAttribArrays();
		// 1rst attribute buffer : vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribIPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_INT,             // type
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glBindBuffer(GL_ARRAY_BUFFER, connectionBuffer);
		glVertexAttribIPointer(
			2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			1,                  // size
			GL_UNSIGNED_BYTE,             // type
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_POINTS, 0, pointsCount);

		disableAttribArrays();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void cleanup()
	{
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &connectionBuffer);
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

	GLuint VertexArrayID, programID, matrixID, camPosID;
	GLuint vertexBuffer, connectionBuffer;
	int pointsCount;

	vector<int> attribIDs = { 0, 2 };

    GLFWwindow* window;
};

#endif
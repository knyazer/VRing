#ifndef GL_HPP
#define GL_HPP

#include <include/buffer.hpp>
#include <include/shader.hpp>

GLuint vertices[] = 
{	
		0, 1,
		2, 3
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

	void compileShaders()
	{
		programID = LoadShaders("shaders/main.vs", "shaders/main.fs");
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

	void makeBuffers()
	{
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// The VBO containing the positions and sizes of the particles
		glGenBuffers(1, &instancingBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, instancingBuffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, g_vlist.size * DATA_SIZE * sizeof(int32_t), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, g_vlist.size * DATA_SIZE * sizeof(int32_t), g_data);
	}

	void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void draw(Camera &camera)
	{
		glUseProgram(programID);

		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &camera.MVP[0][0]);
		glUniform3fv(camPosID, 1, &extract(camera.pos)[0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0, // attribute. No particular reason for 0, but must match the layout in the shader.
			1, // size
			GL_INT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*)0 // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, instancingBuffer);
		glVertexAttribPointer(
			1, // attribute. No particular reason for 1, but must match the layout in the shader.
			DATA_SIZE, // size
			GL_INT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*)0 // array buffer offset
		);

		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 1);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, g_vlist.size);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void cleanup()
	{
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &instancingBuffer);
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

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		compileShaders();

		makeMVP();
		cameraPart();

		return 1;
	}

	GLuint VertexArrayID, programID, matrixID, camPosID;
	GLuint vertexBuffer, instancingBuffer;
	int pointsCount;

    GLFWwindow* window;
};

#endif
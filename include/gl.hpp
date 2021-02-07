#ifndef GL_HPP
#define GL_HPP

#include <include/buffer.hpp>

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

		if (buf.vertexes.size() != 0)
		{
			assert(buf.colors.size() == buf.vertexes.size() && buf.barycentrics.size() == buf.vertexes.size());

			makeVertexBuffer();
			glBufferData(GL_ARRAY_BUFFER, buf.vertexes.size() * sizeof(GLfloat), &buf.vertexes[0], GL_DYNAMIC_DRAW);

			makeColorBuffer();
			glBufferData(GL_ARRAY_BUFFER, buf.colors.size() * sizeof(GLfloat), &buf.colors[0], GL_DYNAMIC_DRAW);

			makeBarycentricBuffer();
			glBufferData(GL_ARRAY_BUFFER, buf.barycentrics.size() * sizeof(GLfloat), &buf.barycentrics[0], GL_DYNAMIC_DRAW);
		}

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

    GLFWwindow* window;
};

#endif
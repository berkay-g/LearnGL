#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <imgui_impl_glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Window
{
private:
	GLFWwindow* window;
	int width;
	int height;
public:
	Window(int width, int height, const char* title, bool resizable = GL_FALSE)
		: width(width), height(height), window(nullptr)
	{
		// Initialize GLFW
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initialize GLFW");
		}

		// Configure GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, resizable);


		// Create a GLFW window
		window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Show the cursor

		// Make the created window the current OpenGL context
		glfwMakeContextCurrent(window);

		// Set GLFW event callbacks
		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
		glfwSetWindowSizeCallback(window, window_size_callback);

		// Set GLFW error callback
		glfwSetErrorCallback(errorCallback);

		// Initialize GLAD
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			glfwTerminate();
			throw std::runtime_error("Failed to initialize GLAD");
		}

		// Set the viewport size
		glViewport(0, 0, width, height);
	}

	~Window()
	{
		// Terminate GLFW when the window is destroyed
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void clear(float x, float y, float z, float w, bool enableGLDepth = true, bool clearImGUIframe = false) const
	{
		if (x > 1.f && x <= 255.f)
			x /= 255.f;
		if (y > 1.f && y <= 255.f)
			y /= 255.f;
		if (z > 1.f && z <= 255.f)
			z /= 255.f;
		if (w > 1.f && w <= 255.f)
			w /= 255.f;
		//clear the color of the window with an alpha value of 1.0f
		glClearColor(x, y, z, w);
		if (clearImGUIframe)
		{
			ImGui_ImplGlfw_NewFrame();
		}
		if (enableGLDepth)
		{
			//enable the GL_DEPTH_TEST to be able to see 3D object correctly
			glEnable(GL_DEPTH_TEST);
			//clear both the color buffer bit and the depth buffer bit
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			//clear just the color buffer bit
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	bool shouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void close()
	{
		glfwSetWindowShouldClose(window, true);
	}

	void pollEvents()
	{
		glfwPollEvents();
	}

	void swapBuffers()
	{
		glfwSwapBuffers(window);
	}

	bool isFullScreen()
	{
		return glfwGetWindowMonitor(window) != nullptr;
	}

	void setFullScreen(bool isFullScreen)
	{

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (isFullScreen)
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		else
			glfwSetWindowMonitor(window, nullptr, (mode->width - 1280) / 2, (mode->height - 720) / 2, 1280, 720, mode->refreshRate);
	}

	int getWidth()
	{
		glfwGetWindowSize(window, &width, &height);
		return width;
	}

	int getHeight()
	{
		glfwGetWindowSize(window, &width, &height);
		return height;
	}

	float getAspectRatio()
	{
		return (float)getWidth() / getHeight();
	}

	GLFWwindow* getWindow() const 
	{
		return window;
	}

private:
	static void errorCallback(int error, const char* description)
	{
		std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
	}

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	static void window_size_callback(GLFWwindow* window, int width, int height)
	{

	}

};
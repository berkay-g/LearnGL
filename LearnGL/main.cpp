#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "primitives.h"

#include "images/brick_texture.h"

void processInput(GLFWwindow* window);

Camera camera;

float currentFrame = static_cast<float>(glfwGetTime());
float lastFrame = currentFrame;
float deltaTime;

#ifdef NDEBUG 
#include <Windows.h> 
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else 
int main(void)
#endif
{
	Window window(1280, 720, "title");
	glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	camera.setSpeed(10.f);

	// Load and compile shaders
	Shader shader(vshader, fshader);

	Cube cubes(1000);

	for (int i = 0; i < static_cast<int>(cubes.getInstanceCount()); i++)
	{
		static int z = 0;
		static int x = 0;
		cubes.setPosition(i, glm::vec3(0.0f + z, -0.5f, 0.0f + x));
		x++;
		if ((i+1) % 10 == 0)
		{
			z++;
			x = 0;
		}
	}

	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices;

	Object obj("resources/models/alliance.obj", 100);
	for (int i = 0; i < static_cast<int>(obj.getInstanceCount()); i++)
	{
		obj.setPosition(i, glm::vec3(1.0 + i, 0.f, 0.0f));
	}
	obj.rotate(1, glm::vec3(0.f, 180.f, 0.f));
	obj.scale(1, glm::vec3(2.f));

	Texture brick_tex(brick_texture, brick_texture_size);
	Texture alliance_tex("resources/images/alliance.png");
	brick_tex.unbind();

	// Main loop...
	while (!window.shouldClose())
	{
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window.getWindow());

		window.clear(0.1f, 0.1f, 0.1f, 1.0f);

		// Activate the shader
		shader.use();

		// Set camera view and projection matrices
		//glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		alliance_tex.bind();
		obj.draw();

		brick_tex.bind();
		cubes.draw();

		// Swap the front and back buffers
		window.swapBuffers();

		// Poll for events
		window.pollEvents();
	}

	return 0;
}

void processInput(GLFWwindow* window)
{
	camera.ProccessKeyboard(0, deltaTime);
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProccessKeyboard(GLFW_KEY_W, deltaTime);
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProccessKeyboard(GLFW_KEY_S, deltaTime);
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProccessKeyboard(GLFW_KEY_D, deltaTime);
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProccessKeyboard(GLFW_KEY_A, deltaTime);
	// Fly up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProccessKeyboard(GLFW_KEY_SPACE, deltaTime);
	// Fly down
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProccessKeyboard(GLFW_KEY_LEFT_CONTROL, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	double xpos, ypos;
	int width, height;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, static_cast<double>(height) / 2.0, static_cast<double>(width) / 2.0);

	camera.ProcessMouseMovement(xpos, ypos, static_cast<float>(height), static_cast<float>(width));
}
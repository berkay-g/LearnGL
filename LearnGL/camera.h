#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#define PI acos(-1)

// Default camera values
// 
// Initial horizontal angle : toward -Z
static float horizontalAngle = static_cast<float>(PI);
// Initial vertical angle : none
static float verticalAngle = 0.0f;
// Initial Field of View
static float initialFoV = 45.0f;

static float mouseSpeed = 0.0005f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
	glm::mat4 ViewMatrix;
	float speed;
public:
	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 position;
public:
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 5.0f))
	{
		speed = 3.0;
		position = pos;
		direction = glm::vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		// Right vector
		right = glm::vec3(
			sin(horizontalAngle - PI / 2.0f),
			0,
			cos(horizontalAngle - PI / 2.0f)
		);

		// Up vector
		up = glm::cross(right, direction);

		ViewMatrix = glm::lookAt(
			position,           // Camera is here
			position + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
		);
	}

	glm::mat4 GetViewMatrix()
	{
		direction = glm::vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		// Right vector
		right = glm::vec3(
			sin(horizontalAngle - PI / 2.0f),
			0,
			cos(horizontalAngle - PI / 2.0f)
		);

		// Up vector
		up = glm::cross(right, direction);

		ViewMatrix = glm::lookAt(
			position,           // Camera is here
			position + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
		);

		return ViewMatrix;
	}

	void ProccessKeyboard(int keyCode, float deltaTime)
	{
		// Move forward
		if (keyCode == GLFW_KEY_W) {
			position += direction * deltaTime * speed;
		}
		// Move backward
		if (keyCode == GLFW_KEY_S) {
			position -= direction * deltaTime * speed;
		}
		// Strafe right
		if (keyCode == GLFW_KEY_D) {
			position += right * deltaTime * speed;
		}
		// Strafe left
		if (keyCode == GLFW_KEY_A) {
			position -= right * deltaTime * speed;
		}
		// Fly up
		if (keyCode == GLFW_KEY_SPACE) {
			position += up * deltaTime * speed;
		}
		// Fly down
		if (keyCode == GLFW_KEY_LEFT_CONTROL) {
			position -= up * deltaTime * speed;
		}


		//// Jump
		//const float jumpForce = 5.0f;
		//const float gravity = 9.8f;
		//static bool isJumping = false;
		//static float jumpStartTime = 0.0f;
		//static float initialHeight = 0.0f;
		//static float jumpVelocity = 0.0f;

		//// Jump
		//if (keyCode == GLFW_KEY_SPACE) {
		//	if (!isJumping) {
		//		isJumping = true;
		//		jumpStartTime = glfwGetTime();
		//		initialHeight = position.y;
		//		jumpVelocity = jumpForce;
		//	}
		//}

		//// Apply gravity and jumping force
		//float jumpElapsedTime = glfwGetTime() - jumpStartTime;
		//float jumpHeight = initialHeight + jumpVelocity * jumpElapsedTime - 0.5f * gravity * jumpElapsedTime * jumpElapsedTime;

		//if (isJumping) {
		//	if (jumpHeight > initialHeight) {
		//		position.y = jumpHeight;
		//	}
		//	else {
		//		position.y = initialHeight;
		//		isJumping = false;
		//	}
		//}
		//else {
		//	// Apply gravity
		//	float fallHeight = initialHeight - 0.5f * gravity * jumpElapsedTime * jumpElapsedTime;
		//	position.y = fallHeight > 0.0f ? fallHeight : 0.0f;
		//}

		//if (CheckCollisionWithCube(glm::vec3(0.f), glm::vec3(1.3f)))
		//	std::cout << "collision ! " << position.x << ' ' << position.z << '\n';

	}

	void ProcessMouseMovement(double xpos, double ypos, float height, float width)
	{
		// Compute new orientation
		horizontalAngle += mouseSpeed * float(height / 2 - xpos);
		verticalAngle += mouseSpeed * float(width / 2 - ypos);
	}

	void setSpeed(float speed)
	{
		this->speed = speed;
	}

	// Collision detection method
	bool CheckCollisionWithCube(const glm::vec3& cubePosition, const glm::vec3& cubeSize) const {
		glm::vec3 cameraPosition = position;

		if (cameraPosition.x >= cubePosition.x - cubeSize.x / 2 &&
			cameraPosition.x <= cubePosition.x + cubeSize.x / 2 &&
			cameraPosition.y >= cubePosition.y - cubeSize.y &&
			cameraPosition.z >= cubePosition.z - cubeSize.z / 2 &&
			cameraPosition.z <= cubePosition.z + cubeSize.z / 2) {
			return true; // Collision detected
		}

		return false; // No collision
	}
};
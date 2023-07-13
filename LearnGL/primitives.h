#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

class Triangle
{
private:
	std::vector<glm::mat4> modelMatrices;
	unsigned int instanceVBO;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int instanceCount;

public:
	Triangle(unsigned int count = 1)
		: instanceCount(count)
	{
		// Set up vertex data
		float vertices[] =
		{
			// positions
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		// Set up indices
		unsigned int indices[] =
		{
			0, 1, 2
		};

		float texCoords[] = {
			0.5f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f
		};


		// Create vertex buffer object
		//unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Create element buffer object
		//unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Set up instance model matrices
		modelMatrices.resize(instanceCount, glm::mat4(1.0f));


		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);

		// Set up vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// Set up texture coordinate attribute
		unsigned int texCoordVBO;
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(1, 0);

		// Set up instance matrix attribute
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}
	}

	~Triangle()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void setPosition(unsigned int index, const glm::vec3& position)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	glm::vec3 getPosition(unsigned int index)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return glm::vec3(-1);
		}

		glm::mat4 modelMatrix = modelMatrices[index];
		glm::vec3 position = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

		return position;
	}

	void rotate(unsigned int index, const glm::vec3& rotationAngle)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void scale(unsigned int index, const glm::vec3& scale)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::scale(modelMatrices[index], scale);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void add(const glm::vec3& position = glm::vec3(0.f, 0.f, 0.f))
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		modelMatrices.push_back(model);
		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void remove(unsigned int index)
	{
		if (index >= 0 && index < instanceCount)
		{
			modelMatrices.erase(modelMatrices.begin() + index);
		}

		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	unsigned int getInstanceCount()
	{
		return instanceCount;
	}

	void draw()
	{
		if (modelMatrices.size() > 0)
		{
			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0, instanceCount);
			glBindVertexArray(0);
		}
	}
};

class Square
{
private:
	std::vector<glm::mat4> modelMatrices;
	unsigned int instanceVBO;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int instanceCount;

public:
	Square(unsigned int count = 1)
		: instanceCount(count)
	{
		// Set up vertex data
		float vertices[] = {
			// positions
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		// Set up indices
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		float texCoords[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};


		// Create vertex buffer object
		//unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Create element buffer object
		//unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Set up instance model matrices
		modelMatrices.resize(instanceCount, glm::mat4(1.0f));

		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);

		// Set up vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// Set up texture coordinate attribute
		unsigned int texCoordVBO;
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(1, 0);

		// Set up instance matrix attribute
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}
	}

	~Square()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void setPosition(unsigned int index, const glm::vec3& position)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	glm::vec3 getPosition(unsigned int index)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return glm::vec3(-1);
		}

		glm::mat4 modelMatrix = modelMatrices[index];
		glm::vec3 position = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

		return position;
	}

	void rotate(unsigned int index, const glm::vec3& rotationAngle)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void scale(unsigned int index, const glm::vec3& scale)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::scale(modelMatrices[index], scale);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void add(const glm::vec3& position = glm::vec3(0.f, 0.f, 0.f))
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		modelMatrices.push_back(model);
		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void remove(unsigned int index)
	{
		if (index >= 0 && index < instanceCount)
		{
			modelMatrices.erase(modelMatrices.begin() + index);
		}

		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	unsigned int getInstanceCount()
	{
		return instanceCount;
	}

	void draw()
	{
		if (modelMatrices.size() > 0)
		{
			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceCount);
			glBindVertexArray(0);
		}
	}
};

class Cube
{
private:
	std::vector<glm::mat4> modelMatrices;
	unsigned int instanceVBO;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int instanceCount;

public:
	Cube(unsigned int count = 1)
		: instanceCount(count)
	{
		// Vertex data
		float vertices[] = {
			// Front face
			-0.5f, -0.5f, 0.5f, // Bottom-left
			0.5f, -0.5f, 0.5f, // Bottom-right
			0.5f, 0.5f, 0.5f, // Top-right
			-0.5f, 0.5f, 0.5f, // Top-left
			// Back face
			-0.5f, -0.5f, -0.5f, // Bottom-left
			0.5f, -0.5f, -0.5f, // Bottom-right
			0.5f, 0.5f, -0.5f, // Top-right
			-0.5f, 0.5f, -0.5f, // Top-left
			// Left face
			-0.5f, 0.5f, 0.5f, // Bottom-right
			-0.5f, -0.5f, 0.5f, // Top-right
			-0.5f, -0.5f, -0.5f, // Top-left
			-0.5f, 0.5f, -0.5f, // Bottom-left
			// Right face
			0.5f, 0.5f, 0.5f, // Bottom-right
			0.5f, -0.5f, 0.5f, // Top-right
			0.5f, -0.5f, -0.5f, // Top-left
			0.5f, 0.5f, -0.5f, // Bottom-left
			// Top face
			-0.5f, 0.5f, 0.5f, // Bottom-left
			0.5f, 0.5f, 0.5f, // Bottom-right
			0.5f, 0.5f, -0.5f, // Top-right
			-0.5f, 0.5f, -0.5f, // Top-left
			// Bottom face
			-0.5f, -0.5f, 0.5f, // Bottom-left
			0.5f, -0.5f, 0.5f, // Bottom-right
			0.5f, -0.5f, -0.5f, // Top-right
			-0.5f, -0.5f, -0.5f // Top-left
		};

		// Set up indices
		unsigned int indices[] = {
			// Front face
			0, 1, 2,
			2, 3, 0,
			// Back face
			4, 5, 6,
			6, 7, 4,
			// Left face
			8, 9, 10,
			10, 11, 8,
			// Right face
			12, 13, 14,
			14, 15, 12,
			// Top face
			16, 17, 18,
			18, 19, 16,
			// Bottom face
			20, 21, 22,
			22, 23, 20
		};

		float texCoords[] = {
			// Front face
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			// Back face
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			// Left face
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			// Right face
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			// Top face
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			// Bottom face
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};


		// Create vertex buffer object
		//unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Create element buffer object
		//unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Set up instance model matrices
		modelMatrices.resize(instanceCount, glm::mat4(1.0f));

		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);

		// Set up vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// Set up texture coordinate attribute
		unsigned int texCoordVBO;
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(1, 0);

		// Set up instance matrix attribute
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}

	}

	~Cube()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void setPosition(unsigned int index, const glm::vec3& position)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	glm::vec3 getPosition(unsigned int index)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return glm::vec3(-1);
		}

		glm::mat4 modelMatrix = modelMatrices[index];
		glm::vec3 position = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

		return position;
	}

	void rotate(unsigned int index, const glm::vec3& rotationAngle)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void scale(unsigned int index, const glm::vec3& scale)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::scale(modelMatrices[index], scale);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void add(const glm::vec3& position = glm::vec3(0.f, 0.f, 0.f))
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		modelMatrices.push_back(model);
		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void remove(unsigned int index)
	{
		if (index >= 0 && index < instanceCount)
		{
			modelMatrices.erase(modelMatrices.begin() + index);
		}

		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	unsigned int getInstanceCount()
	{
		return instanceCount;
	}

	void draw()
	{
		if (modelMatrices.size() > 0)
		{
			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instanceCount);
			glBindVertexArray(0);
		}
	}
};

class Pyramid
{
private:
	std::vector<glm::mat4> modelMatrices;
	unsigned int instanceVBO;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int instanceCount;

public:
	Pyramid(unsigned int count = 1)
		: instanceCount(count)
	{
		// Vertex data
		float vertices[] = {
			// Front face
			-0.5f, -0.5f, 0.5f, // Bottom-left
			 0.5f, -0.5f, 0.5f, // Bottom-right
			 0.0f,  0.5f, 0.0f, // Top

			 // Left face
			 -0.5f, -0.5f, 0.5f, // Bottom-right
			 -0.5f, -0.5f, -0.5f, // Bottom-left
			  0.0f,  0.5f, 0.0f, // Top

			  // Right face
			   0.5f, -0.5f, 0.5f, // Bottom-left
			   0.5f, -0.5f, -0.5f, // Bottom-right
			   0.0f,  0.5f, 0.0f, // Top

			   // Back face
			   -0.5f, -0.5f, -0.5f, // Bottom-right
				0.5f, -0.5f, -0.5f, // Bottom-left
				0.0f,  0.5f, 0.0f, // Top
		};

		// Index data
		unsigned int indices[] = {
			// Front face
			0, 1, 2,

			// Left face
			3, 4, 5,

			// Right face
			6, 7, 8,

			// Back face
			9, 10, 11
		};

		float texCoords[] = {
			// Base
			0.5f, 1.0f, // Vertex 0
			1.0f, 0.0f, // Vertex 1
			0.0f, 0.0f, // Vertex 2

			0.5f, 1.0f, // Vertex 0
			0.0f, 0.0f, // Vertex 2
			0.0f, 1.0f, // Vertex 3

			// Front face
			0.5f, 1.0f, // Vertex 0
			1.0f, 0.0f, // Vertex 4
			0.0f, 0.0f, // Vertex 1

			// Right face
			0.5f, 1.0f, // Vertex 0
			1.0f, 0.0f, // Vertex 1
			0.0f, 0.0f, // Vertex 2

			// Back face
			0.5f, 1.0f, // Vertex 0
			1.0f, 0.0f, // Vertex 2
			0.0f, 0.0f, // Vertex 3

			// Left face
			0.5f, 1.0f, // Vertex 0
			1.0f, 0.0f, // Vertex 3
			0.0f, 0.0f, // Vertex 4
		};



		// Create vertex buffer object
		//unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Create element buffer object
		//unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Set up instance model matrices
		modelMatrices.resize(instanceCount, glm::mat4(1.0f));

		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);

		// Set up vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// Set up texture coordinate attribute
		unsigned int texCoordVBO;
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(1, 0);

		// Set up instance matrix attribute
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}
	}

	~Pyramid()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void setPosition(unsigned int index, const glm::vec3& position)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	glm::vec3 getPosition(unsigned int index)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return glm::vec3(-1);
		}

		glm::mat4 modelMatrix = modelMatrices[index];
		glm::vec3 position = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

		return position;
	}

	void rotate(unsigned int index, const glm::vec3& rotationAngle)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void scale(unsigned int index, const glm::vec3& scale)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::scale(modelMatrices[index], scale);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void add(const glm::vec3& position = glm::vec3(0.f, 0.f, 0.f))
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		modelMatrices.push_back(model);
		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void remove(unsigned int index)
	{
		if (index >= 0 && index < instanceCount)
		{
			modelMatrices.erase(modelMatrices.begin() + index);
		}

		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	unsigned int getInstanceCount()
	{
		return instanceCount;
	}

	void draw()
	{
		if (modelMatrices.size() > 0)
		{
			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0, instanceCount);
			glBindVertexArray(0);
		}
	}
};

class Object
{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int vertexCount;
	unsigned int indexCount;
	std::vector<glm::mat4> modelMatrices;
	unsigned int instanceVBO;
	unsigned int instanceCount;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
public:
	Object(const char* objPath, unsigned int count = 1)
		: instanceCount(count)
	{
		loadObj(objPath);

		// Create vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create vertex buffer object
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Create element buffer object
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Set up instance model matrices
		modelMatrices.resize(instanceCount, glm::mat4(1.0f));

		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);

		// Set up instance matrix attribute
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}

		// Unbind VAO
		glBindVertexArray(0);

		// Store the number of vertices and indices
		vertexCount = static_cast<int>(vertices.size());
		indexCount = static_cast<int>(indices.size());
	}

	Object(std::string& objData, unsigned int count = 1)
		: instanceCount(count)
	{
		loadObjStr(objData);


		// Create vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create vertex buffer object
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Create element buffer object
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Set up instance model matrices
		modelMatrices.resize(instanceCount, glm::mat4(1.0f));

		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);

		// Set up instance matrix attribute
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}

		// Unbind VAO
		glBindVertexArray(0);

		// Store the number of vertices and indices
		vertexCount = static_cast<int>(vertices.size());
		indexCount = static_cast<int>(indices.size());
	}

	~Object()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void setPosition(unsigned int index, const glm::vec3& position)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	glm::vec3 getPosition(unsigned int index)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return glm::vec3(-1);
		}

		glm::mat4 modelMatrix = modelMatrices[index];
		glm::vec3 position = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

		return position;
	}

	void rotate(unsigned int index, const glm::vec3& rotationAngle)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void scale(unsigned int index, const glm::vec3& scale)
	{
		if (index >= instanceCount || index < 0)
		{
			std::cerr << "Invalid index: " << index << std::endl;
			return;
		}

		modelMatrices[index] = glm::scale(modelMatrices[index], scale);
		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void add(const glm::vec3& position = glm::vec3(0.f, 0.f, 0.f))
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		modelMatrices.push_back(model);
		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	void remove(unsigned int index)
	{
		if (index >= 0 && index < instanceCount)
		{
			modelMatrices.erase(modelMatrices.begin() + index);
		}

		instanceCount = static_cast<int>(modelMatrices.size());

		if (modelMatrices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		}
	}

	unsigned int getInstanceCount()
	{
		return instanceCount;
	}

	void draw()
	{
		if (modelMatrices.size() > 0)
		{
			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount);
			glBindVertexArray(0);
		}
	}

private:
	void loadObj(const char* objPath)
	{
		std::ifstream file(objPath);
		std::string line;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string type;
			iss >> type;

			if (type == "v")
			{
				glm::vec3 position;
				iss >> position.x >> position.y >> position.z;
				positions.push_back(position);
			}
			else if (type == "vt")
			{
				glm::vec2 texCoord;
				iss >> texCoord.x >> texCoord.y;
				texCoords.push_back(texCoord);
			}
			else if (type == "vn")
			{
				glm::vec3 normal;
				iss >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f")
			{
				for (int i = 0; i < 3; ++i)
				{
					std::string vertexData;
					iss >> vertexData;
					std::istringstream vdiss(vertexData);

					std::string positionIndex, texCoordIndex, normalIndex;
					std::getline(vdiss, positionIndex, '/');
					std::getline(vdiss, texCoordIndex, '/');
					std::getline(vdiss, normalIndex, '/');

					unsigned int posIndex = std::stoi(positionIndex) - 1;
					unsigned int texIndex = std::stoi(texCoordIndex) - 1;
					unsigned int normIndex = std::stoi(normalIndex) - 1;

					Vertex vertex;
					vertex.position = positions[posIndex];
					vertex.texCoord = texCoords[texIndex];
					vertex.normal = normals[normIndex];

					vertices.push_back(vertex);
					indices.push_back(static_cast<unsigned int>(indices.size()));
				}
			}
		}
	}

	void loadObjStr(std::string& objData)
	{
		std::istringstream file(objData);
		std::string line;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string type;
			iss >> type;

			if (type == "v")
			{
				glm::vec3 position;
				iss >> position.x >> position.y >> position.z;
				positions.push_back(position);
			}
			else if (type == "vt")
			{
				glm::vec2 texCoord;
				iss >> texCoord.x >> texCoord.y;
				texCoords.push_back(texCoord);
			}
			else if (type == "vn")
			{
				glm::vec3 normal;
				iss >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f")
			{
				for (int i = 0; i < 3; ++i)
				{
					std::string vertexData;
					iss >> vertexData;
					std::istringstream vdiss(vertexData);

					std::string positionIndex, texCoordIndex, normalIndex;
					std::getline(vdiss, positionIndex, '/');
					std::getline(vdiss, texCoordIndex, '/');
					std::getline(vdiss, normalIndex, '/');

					unsigned int posIndex = std::stoi(positionIndex) - 1;
					unsigned int texIndex = std::stoi(texCoordIndex) - 1;
					unsigned int normIndex = std::stoi(normalIndex) - 1;

					Vertex vertex;
					vertex.position = positions[posIndex];
					vertex.texCoord = texCoords[texIndex];
					vertex.normal = normals[normIndex];

					vertices.push_back(vertex);
					indices.push_back(static_cast<unsigned int>(indices.size()));
				}
			}
		}
	}
};

class Model
{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int vertexCount;
	unsigned int indexCount;
	std::vector<glm::mat4> modelMatrices;
	unsigned int instanceVBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int updateCall;

	bool somethingChanged = false;
public:
	Model(const char* objPath, std::vector<glm::mat4>& modelMatrices)
		: modelMatrices(modelMatrices)
	{
		loadObj(objPath);

		// Create vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create vertex buffer object
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Create element buffer object
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);
		updateCall = 1;

		// Set up instance matrix attribute
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}

		// Unbind VAO
		glBindVertexArray(0);

		// Store the number of vertices and indices
		vertexCount = static_cast<int>(vertices.size());
		indexCount = static_cast<int>(indices.size());
	}

	Model(std::string& objData, std::vector<glm::mat4>& modelMatrices)
		: modelMatrices(modelMatrices)
	{
		loadObjStr(objData);

		// Create vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create vertex buffer object
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Create element buffer object
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Create instance matrix buffer object
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);
		updateCall = 1;

		// Set up instance matrix attribute
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}

		// Unbind VAO
		glBindVertexArray(0);

		// Store the number of vertices and indices
		vertexCount = static_cast<int>(vertices.size());
		indexCount = static_cast<int>(indices.size());
	}

	~Model()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void updateMatrices(bool force = false)
	{
		if(!force)
			if (!somethingChanged) return;
		somethingChanged = false;

		updateCall++;
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	int getMatricesSize()
	{
		return static_cast<int>(modelMatrices.size());
	}

	void add(const glm::vec3& position = glm::vec3(0.f), const glm::vec3& scale = glm::vec3(0.f))
	{
		glm::mat4 model = glm::translate(glm::mat4(1.f), position);
		model = glm::scale(model, scale);

		modelMatrices.push_back(model);
		updateMatrices();
	}

	int updateCallAmount()
	{
		return updateCall;
	}

	void add(glm::mat4& model)
	{
		modelMatrices.push_back(model);
		updateMatrices();
	}

	void remove(int index)
	{
		if (index >= 0 && index < modelMatrices.size())
		{
			modelMatrices.erase(modelMatrices.begin() + index);
		}
		somethingChanged = true;
	}

	void draw()
	{
		if (modelMatrices.size() > 0)
		{
			glBindVertexArray(VAO);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(modelMatrices.size()));
			glBindVertexArray(0);
		}
	}

	glm::mat4* operator[](int i)
	{
		if (modelMatrices.size() > 0)
			return &modelMatrices[i];
		else
			return nullptr;
	}

	bool validIndex(int index) const
	{
		if (index >= 0 && index < modelMatrices.size())
			return true;
		return false;
	}

	glm::vec3 getPosition(int i)
	{
		if (validIndex(i))
			return getPositionFromModelMatrix(modelMatrices[i]);
		return { 0.f, 0.f, 0.f };
	}

	glm::vec3 getScale(int i) const
	{
		if (!validIndex(i))
			return { 0.f, 0.f, 0.f };
		glm::vec3 scale;
		glm::vec3 position = modelMatrices[i][3]; // 4th column of the model matrix
		glm::mat4 model = modelMatrices[i];

		scale.x = glm::length(glm::vec3(model[0])); // Basis vector X
		scale.y = glm::length(glm::vec3(model[1])); // Basis vector Y
		scale.z = glm::length(glm::vec3(model[2])); // Basis vector Z
		return scale;
	}

	glm::vec3 getRotation(int i) const
	{
		if (!validIndex(i))
			return { 0.f, 0.f, 0.f };

		const glm::vec3 left = glm::normalize(glm::vec3(modelMatrices[i][0])); // Normalized left axis
		const glm::vec3 up = glm::normalize(glm::vec3(modelMatrices[i][1])); // Normalized up axis
		const glm::vec3 forward = glm::normalize(glm::vec3(modelMatrices[i][2])); // Normalized forward axis

		// Obtain the "unscaled" transform matrix
		glm::mat4 m(0.0f);
		m[0][0] = left.x;
		m[0][1] = left.y;
		m[0][2] = left.z;

		m[1][0] = up.x;
		m[1][1] = up.y;
		m[1][2] = up.z;

		m[2][0] = forward.x;
		m[2][1] = forward.y;
		m[2][2] = forward.z;

		glm::vec3 rot;
		rot.x = atan2f(m[1][2], m[2][2]);
		rot.y = atan2f(-m[0][2], sqrtf(m[1][2] * m[1][2] + m[2][2] * m[2][2]));
		rot.z = atan2f(m[0][1], m[0][0]);
		rot = glm::degrees(rot); // Convert to degrees, or you could multiply it by (180.f / 3.14159265358979323846f)
		return rot;
	}

	void setPosition(unsigned int index, const glm::vec3& position)
	{
		if (!validIndex(index))
		{
			std::cerr << "Position Invalid index: " << index << std::endl;
			return;
		}
		if (modelMatrices[index] == glm::translate(glm::mat4(1.0f), position))
			return;

		somethingChanged = true;
		modelMatrices[index] = glm::translate(glm::mat4(1.0f), position);
	}

	void changeTransform(int i, const glm::mat4& model)
	{
		if (modelMatrices[i] == model || !validIndex(i))
			return;
		somethingChanged = true;
		modelMatrices[i] = model;
	}

	void changeTransform(int i, const glm::vec3& position, const glm::vec3& rotation, const float rotAngle, const glm::vec3& scale)
	{
		if (!validIndex(i))
			return;
		if (modelMatrices[i] == glm::translate(glm::mat4(1.f), position) &&
			modelMatrices[i] == glm::scale(modelMatrices[i], glm::vec3(scale)) &&
			modelMatrices[i] == glm::rotate(modelMatrices[i], glm::radians(rotAngle), rotation))
			return;
		somethingChanged = true;
		modelMatrices[i] = glm::translate(glm::mat4(1.f), position);
		modelMatrices[i] = glm::scale(modelMatrices[i], glm::vec3(scale));
		modelMatrices[i] = glm::rotate(modelMatrices[i], glm::radians(rotAngle), rotation);
	}

	void setRotation(unsigned int index, const glm::vec3& rotationAngle)
	{
		if (!validIndex(index))
		{
			std::cerr << "Rotate Invalid index: " << index << std::endl;
			return;
		}
		if (modelMatrices[index] == glm::rotate(modelMatrices[index], glm::radians(rotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f)) &&
			modelMatrices[index] == glm::rotate(modelMatrices[index], glm::radians(rotationAngle.y), glm::vec3(1.0f, 0.0f, 0.0f)) && 
			modelMatrices[index] == glm::rotate(modelMatrices[index], glm::radians(rotationAngle.z), glm::vec3(1.0f, 0.0f, 0.0f)))
			return;

		somethingChanged = true;
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrices[index] = glm::rotate(modelMatrices[index], glm::radians(rotationAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void setRotation(unsigned int index, const float rotAngle, const glm::vec3& rotationAxis)
	{
		if (!validIndex(index))
		{
			std::cerr << "Rotate Invalid index: " << index << std::endl;
			return;
		}
		if (modelMatrices[index] == glm::rotate(modelMatrices[index], glm::radians(rotAngle), rotationAxis))
			return;

		somethingChanged = true;
		modelMatrices[index] == glm::rotate(modelMatrices[index], rotAngle, rotationAxis);
	}

	void setScale(unsigned int index, const glm::vec3& scale)
	{
		if (!validIndex(index))
		{
			std::cerr << "Scale Invalid index: " << index << std::endl;
			return;
		}
		if (modelMatrices[index] == glm::scale(modelMatrices[index], scale))
			return;

		somethingChanged = true;
		modelMatrices[index] = glm::scale(modelMatrices[index], scale);
	}

	void setScale(unsigned int index, const float& scale)
	{
		if (!validIndex(index))
		{
			std::cerr << "Scale Invalid index: " << index << std::endl;
			return;
		}
		if (modelMatrices[index] == glm::scale(modelMatrices[index], glm::vec3(scale)))
			return;

		somethingChanged = true;
		modelMatrices[index] = glm::scale(modelMatrices[index], glm::vec3(scale));
	}

private:
	void loadObj(const char* objPath)
	{
		std::ifstream file(objPath);
		std::string line;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string type;
			iss >> type;

			if (type == "v")
			{
				glm::vec3 position;
				iss >> position.x >> position.y >> position.z;
				positions.push_back(position);
			}
			else if (type == "vt")
			{
				glm::vec2 texCoord;
				iss >> texCoord.x >> texCoord.y;
				texCoords.push_back(texCoord);
			}
			else if (type == "vn")
			{
				glm::vec3 normal;
				iss >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f")
			{
				for (int i = 0; i < 3; ++i)
				{
					std::string vertexData;
					iss >> vertexData;
					std::istringstream vdiss(vertexData);

					std::string positionIndex, texCoordIndex, normalIndex;
					std::getline(vdiss, positionIndex, '/');
					std::getline(vdiss, texCoordIndex, '/');
					std::getline(vdiss, normalIndex, '/');

					unsigned int posIndex = std::stoi(positionIndex) - 1;
					unsigned int texIndex = std::stoi(texCoordIndex) - 1;
					unsigned int normIndex = std::stoi(normalIndex) - 1;

					Vertex vertex;
					vertex.position = positions[posIndex];
					vertex.texCoord = texCoords[texIndex];
					vertex.normal = normals[normIndex];

					vertices.push_back(vertex);
					indices.push_back(static_cast<unsigned int>(indices.size()));
				}
			}
		}
	}

	void loadObjStr(std::string& objData)
	{
		std::istringstream file(objData);
		std::string line;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string type;
			iss >> type;

			if (type == "v")
			{
				glm::vec3 position;
				iss >> position.x >> position.y >> position.z;
				positions.push_back(position);
			}
			else if (type == "vt")
			{
				glm::vec2 texCoord;
				iss >> texCoord.x >> texCoord.y;
				texCoords.push_back(texCoord);
			}
			else if (type == "vn")
			{
				glm::vec3 normal;
				iss >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f")
			{
				for (int i = 0; i < 3; ++i)
				{
					std::string vertexData;
					iss >> vertexData;
					std::istringstream vdiss(vertexData);

					std::string positionIndex, texCoordIndex, normalIndex;
					std::getline(vdiss, positionIndex, '/');
					std::getline(vdiss, texCoordIndex, '/');
					std::getline(vdiss, normalIndex, '/');

					unsigned int posIndex = std::stoi(positionIndex) - 1;
					unsigned int texIndex = std::stoi(texCoordIndex) - 1;
					unsigned int normIndex = std::stoi(normalIndex) - 1;

					Vertex vertex;
					vertex.position = positions[posIndex];
					vertex.texCoord = texCoords[texIndex];
					vertex.normal = normals[normIndex];

					vertices.push_back(vertex);
					indices.push_back(static_cast<unsigned int>(indices.size()));
				}
			}
		}
	}

	glm::vec3 getPositionFromModelMatrix(const glm::mat4& modelMatrix) const
	{
		return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
	}
};

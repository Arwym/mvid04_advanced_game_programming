#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"

#pragma region Functions: for Main Loop

void onChangeFrameBufferSize(GLFWwindow *window, const int32_t width, const int32_t height)
{
	glad_glViewport(0, 0, width, height);
}

void handleInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void updateColor(const Shader& shader, double& last_time, char& hue)
{
	const auto current_time = glfwGetTime();
	const float value = 1.0f;
	glm::vec3 color;

	// Change hue every quarter of a second
	if (current_time - last_time > 0.25)
	{
		last_time = current_time;

		if (hue == 'r')
		{
			hue = 'g';
		}
		else if (hue == 'g')
		{
			hue = 'b';
		}
		else
		{
			hue = 'r';
		}
	}

	// Set color
	if (hue == 'r')
	{
		color = glm::vec3(0.0f, value, 0.0f);
	}
	else if (hue == 'g')
	{
		color = glm::vec3(0.0f, 0.0f, value);
	}
	else
	{
		color = glm::vec3(value, 0.0f, 0.0f);
	}

	shader.set("uColor", color);
}

void render(const uint32_t& VAO, const Shader& shader, double& last_time, char &hue) {
	glClear(GL_COLOR_BUFFER_BIT);
	shader.use();
	updateColor(shader, last_time, hue);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

uint32_t createVertexData(uint32_t *VBO, uint32_t *EBO)
{
	float vertices[] = { // triangle vertex attributes: position (0) and color (1)
		0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f
	};

	uint32_t indices[] = { // vertex relationships
		0, 2, 1
	};

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(VAO);

	// Bind buffers and assign data
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set vertex attribute pointer at 0 (position attribute), then enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set vertex attribute pointer at 1 (color attribute), then enable
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind buffers and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // NOTE: must happen after unbinding VAO

	return VAO;
}

#pragma endregion

int main(int argc, char* argv[])
{
	if (!glfwInit()) // Initialize the GLFW library
	{
		std::cout << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use Core Profile

	// Create new window in windowed mode and set its OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 600, "New Window", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed t create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Initialize GLAD
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	// Set callback for window resize
	glfwSetFramebufferSizeCallback(window, onChangeFrameBufferSize);

	// Create program and vertex data
	const Shader shader("../tests/EJ_03_02/vertex.vs", "../tests/EJ_03_02/fragment.fs");
	uint32_t VBO, EBO; // vertex and element buffer objects
	uint32_t VAO = createVertexData(&VBO, &EBO); // vertex array object

	// Set window bg color
	glClearColor(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);

	// Initialize triangle color info and time counter
	char hue = 'r';
	double last_time = glfwGetTime();

	// Set polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // NOTE: use GL_LINE to use "WIREFRAME MODE" instead

	// Enable cull faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // don't draw back faces

	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		handleInput(window);

		// Render
		render(VAO, shader, last_time, hue);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Delete everything before terminating
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


	// Exit
	glfwTerminate();

	return 0;
}

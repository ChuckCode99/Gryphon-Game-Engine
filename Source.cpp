#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Texture.hpp"

#include "Camera.hpp"


// stores how much we're seeing of either texture
float mixValue = 0.2f;

glm::vec3 ObjectColor(0.25f, 0.55f, 0.05f);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

class program
{
public:
	program()
		: WINDOW_WIDTH(800), WINDOW_HEIGHT(600) //,
		  // lastX(WINDOW_WIDTH / 2.0f), lastY(WINDOW_HEIGHT / 2.0f), firstMouse(true),
		  // deltaTime(0.0f), lastFrame(0.0f)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Project", nullptr, nullptr);

		if (!window)
		{
			std::cout << "Failed to create GLFW window" << std::endl;

			glfwTerminate();
			// Throw exception
		}


		glfwMakeContextCurrent(window);


		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			// Throw exception
		}

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	}

	void processInput(GLFWwindow* window);


	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	void run()
	{
		extern float cube_vertices [108 + 108 + 82];


		shader Shader("VertexShader.vs", "FragmentShader.fs");
		shader LightingCubeShader("LightingVertexShader.vs", "LightingFragmentShader.fs");

		glfwSetWindowUserPointer(window, this);

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) 
			{
				auto self = static_cast<program*>(glfwGetWindowUserPointer(window));
				self->framebuffer_size_callback(window, w, h);
			});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y)
			{
				auto self = static_cast<program*>(glfwGetWindowUserPointer(window));
				self->scroll_callback(window, x, y);
			});
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y)
			{
				auto self = static_cast<program*>(glfwGetWindowUserPointer(window));
				self->mouse_callback(window, x, y);
			});

		unsigned int CubeVAO, LightCubeVAO, VBO;

		glGenVertexArrays(1, &CubeVAO);
		glBindVertexArray(CubeVAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		texture Texture0("awesomeface.png", "png");
		texture Texture1("container.jpg", "jpg");

		glGenVertexArrays(1, &LightCubeVAO);
		glBindVertexArray(LightCubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);



		Shader.use();

		Shader.setInt("texture0", Texture0.TextureLayer);
		Shader.setInt("texture1", Texture1.TextureLayer);


		glm::vec3 lightPos(2.0f, -1.0f, 3.0f);

		glEnable(GL_DEPTH_TEST);

		while (!glfwWindowShouldClose(window))
		{
			// per-frame time logic
			// --------------------
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// input
			// -----
			processInput(window);

			// render
			// ------
			glClearColor(0.1f, 0.09f, 0.2f, 1.0f); 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);


			Shader.use();

			Shader.setVec3("objectColor", ObjectColor);
			Shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
			Shader.setVec3("lightPos", lightPos);
			Shader.setVec3("viewPos", camera.Position);

			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.95f, -0.65f, -0.4f));
			view = camera.GetViewMatrix();
			projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

			Shader.setMat4("model", model);
			Shader.setMat4("view", view);
			Shader.setMat4("projection", projection);

			Texture0.Activate_Bind();
			Texture1.Activate_Bind();
			Shader.setFloat("mixValue", mixValue);

			glBindVertexArray(CubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);


			LightingCubeShader.use();
			LightingCubeShader.setMat4("projection", projection);
			LightingCubeShader.setMat4("view", view);
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.2f, 0.45f, 0.3f));
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			LightingCubeShader.setMat4("model", model);
			
			// Shader.setMat4("lightmodel", model);
			// Shader.setMat4("lightview", view);
			// Shader.setMat4("lightprojection", projection);
			
			lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
			lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

			glBindVertexArray(LightCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);


			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	~program()
	{
		glfwTerminate();
	}
private:
	// settings
	const unsigned int WINDOW_WIDTH, WINDOW_HEIGHT;
	GLFWwindow* window;

	// // camera
	// Camera camera;
	// float lastX;
	// float lastY;
	// bool firstMouse;
	// 
	// // timing
	// float deltaTime = 0.0f;	// time between current frame and last frame
	// float lastFrame = 0.0f;
};

int main()
{
	program Program;

	Program.run();

	return 0;
}

bool isKeyPressed(GLFWwindow* window, int key)
{
	static int oldState = GLFW_RELEASE;
	int newState = glfwGetKey(window, key);
	bool isPressed {false};

	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
		isPressed = true;

	oldState = newState;

	return isPressed;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void program::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
		
		if (mixValue >= 1.0f) mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
		
		if (mixValue <= 0.0f) mixValue = 0.0f;
	}
	if (isKeyPressed(window, GLFW_KEY_C))
		if (ObjectColor == glm::vec3(0.25f, 0.55f, 0.05f))
			ObjectColor = glm::vec3(1.0f, 1.0f, 1.0f);
		else
			ObjectColor = glm::vec3(0.25f, 0.55f, 0.05f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void program::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void program::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void program::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

float cube_vertices[]
{
	    // position           // lighting          // texture
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
												  
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};
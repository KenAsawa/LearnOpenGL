#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other includes
#include "nanogui/nanogui.h"

//Shader stuff
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

//GUI stuff
using namespace nanogui;
// Window dimensions
const GLuint width = 800, height = 600;
Color colval(0.5f, 0.5f, 0.7f, 1.f);
float cameraX = 0;
float cameraY = 0;
float cameraZ = 0;

float rotateValue = 0;

float zNear = 0;
float zFar = 0;
std::string modelName = "A string";


Screen* screen = nullptr;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Initialize GLFW to version 3.3
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(width, height, "Assignment 0", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
#if defined(NANOGUI_GLAD)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Could not initialize GLAD!");
	glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
#endif

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Create a nanogui screen and pass the glfw pointer to initialize
	screen = new Screen();
	screen->initialize(window, true);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glfwSwapBuffers(window);

	// Create nanogui gui
	bool enabled = true;
	FormHelper* gui = new FormHelper(screen);
	ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Controls");
	gui->addGroup("Color");
	gui->addVariable("Object Color:", colval);
	
	gui->addGroup("Position");
	gui->addVariable("X", cameraX)->setSpinnable(true);
	gui->addVariable("Y", cameraY)->setSpinnable(true);
	gui->addVariable("Z", cameraZ)->setSpinnable(true);

	gui->addGroup("Rotate");
	gui->addVariable("Rotate Value", rotateValue)->setSpinnable(true);
	gui->addButton("Rotate right+", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
	gui->addButton("Rotate right-", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
	gui->addButton("Rotate up+", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
	gui->addButton("Rotate up-", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
	gui->addButton("Rotate front+", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
	gui->addButton("Rotate front-", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;

	gui->addGroup("Configuration");
	gui->addVariable("Z Near", zNear)->setSpinnable(true);
	gui->addVariable("Z Far", zFar)->setSpinnable(true);
	gui->addVariable("Model Name", modelName);
	gui->addButton("Reload model", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
	gui->addButton("Reset Camera", []() {
		//TODO
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
	
	screen->setVisible(true);
	screen->performLayout();

	glfwSetCursorPosCallback(window,
		[](GLFWwindow*, double x, double y) {
			screen->cursorPosCallbackEvent(x, y);
		}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow*, int button, int action, int modifiers) {
			screen->mouseButtonCallbackEvent(button, action, modifiers);
		}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow*, int key, int scancode, int action, int mods) {
			screen->keyCallbackEvent(key, scancode, action, mods);
		}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow*, unsigned int codepoint) {
			screen->charCallbackEvent(codepoint);
		}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow*, int count, const char** filenames) {
			screen->dropCallbackEvent(count, filenames);
		}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow*, double x, double y) {
			screen->scrollCallbackEvent(x, y);
		}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow*, int width, int height) {
			screen->resizeCallbackEvent(width, height);
		}
	);
	//End of nanogui gui

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	//
	// SHADER
	//

	// Creates UID for shader, stores the source, then compiles it
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::cout << vertexShader << std::endl;
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checks if vertexShader compiled succesfully
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Repeats process above to compile fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Checks if fragmentShader succesfully compiled
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Creates shader program to link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Checks if shader program linked shaders succesfully
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// Clean up resources we dont need anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Set up vertex data on a normalized plane from -1 to 1, (x, y, z)
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	// VBO is a list, VAO is a container
	unsigned int VBO, VAO;

	// Create 1 UID each for the list and container
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Sets container and list to be modifiable
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copies data into VBO, and the VBO is automatically in the VAO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Tells OpenGL how to read the data in VBO and then enables it
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// glVertexAttribPointer bound VBO to the VAO, so we can unbind the VBO now so we don't modify it
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO so we don't modify it
	glBindVertexArray(0);

	// Game Loop
	while (!glfwWindowShouldClose(window)) {
		// get input
		processInput(window);

		// render events
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Uses shader, binds and unbinds VAO
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		// draw a triangle with starting index and vertices
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Draws GUI
		screen->drawWidgets();

		// double buffer and input events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up remaining resources
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	// Checks if the key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Resizes the window
	glViewport(0, 0, width, height);
}
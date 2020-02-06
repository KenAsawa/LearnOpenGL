// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include "nanogui/nanogui.h"
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers);
void key_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow*, unsigned int codepoint);
void load_model(const char* pathName);
void resetVariables();

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// Model Stuff
unsigned int VBO, VAO;
Model* model = nullptr;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//GUI stuff
using namespace nanogui;
Color colval(0.5f, 0.5f, 0.7f, 1.f);
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 0.0f;

int cameraYaw = -90;
int cameraPitch = 0;
int cameraRoll = 0;

float zNear = 0.4f;
float zFar = 5.0f;

enum test_enum {
	Item1,
	Item2,
	Item3
};
test_enum renderType = test_enum::Item2;
test_enum cullingType = test_enum::Item2;
std::string modelName = "cyborg.obj";

Screen* screen = nullptr;

int main() {
	// Initializes GLFW to version 3.3
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates a window object
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initializes GLAD
	#if defined(NANOGUI_GLAD)
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Could not initialize GLAD!");
		}
		glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
	#endif

	// Sets OpenGL Screen Size and register screen resize callback
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, char_callback);

	// Creates a nanogui screen and pass the glfw pointer to initialize
	screen = new Screen();
	screen->initialize(window, true);

	// Start of nanogui gui
	bool enabled = true;
	FormHelper* gui = new FormHelper(screen);
	ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Controls");
	gui->addGroup("Model Color");
	gui->addVariable("Object Color:", colval);

	gui->addGroup("Camera Position");
	gui->addVariable("X", cameraX)->setSpinnable(true);
	gui->addVariable("Y", cameraY)->setSpinnable(true);
	gui->addVariable("Z", cameraZ)->setSpinnable(true);

	gui->addGroup("Camera Rotatation");
	gui->addVariable("Yaw", cameraYaw)->setSpinnable(true);
	gui->addVariable("Pitch", cameraPitch)->setSpinnable(true);
	gui->addVariable("Roll", cameraRoll)->setSpinnable(true);

	gui->addGroup("Configuration");
	gui->addVariable("Z Near", zNear)->setSpinnable(true);
	gui->addVariable("Z Far", zFar)->setSpinnable(true);
	gui->addVariable("Render Type", renderType, enabled)->setItems({ "Point", "Line", "Triangle" });
	gui->addVariable("Culling Type", cullingType, enabled)->setItems({ "CW", "CCW" });
	gui->addVariable("Model Name", modelName);
	gui->addButton("Reload model", []() {
		// Loads inputted model
		std::string pathName = "resources/objects/" + modelName;
		load_model(pathName.c_str());
		resetVariables();
		});
	gui->addButton("Reset Camera", []() {
		// Resets all variables to base values.
		resetVariables();
		});
	screen->setVisible(true);
	screen->performLayout();
	//End of nanogui gui

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
	glEnable(GL_DEPTH_TEST);
	// Sets size of points when rendering as points
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(2.0);
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Builds and compiles shaders
	Shader shader("shader.vs", "shader.fs");

	// Loads model
	model = new Model();
	load_model("resources/objects/cyborg.obj");

	// Game Loop
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Gets input
		processInput(window);

		// Renders events
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE); //Activates back-face culling.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Sets functionality of colorpicker GUI
		shader.use();
		shader.setVec3("ourColor", colval.r(), colval.g(), colval.b());

		// Passes projection matrix to shader
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, zNear, zFar);
		shader.setMatrix("projection", projection);
		
		//Apply camera translation and rotation.
		camera.TranslateCamera(cameraX, cameraY, cameraZ);
		camera.RotateCamera(cameraYaw, cameraPitch, cameraRoll);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMatrix("view", view);

		// Sets culling mode for model
		if (cullingType == 0) {
			glFrontFace(GL_CW); // Renders CW
		}
		else {
			glFrontFace(GL_CCW); // Renders CCW
		}

		// Sets render mode
		if (renderType == 0) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // Render as points
		} else if (renderType == 1) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Render as lines
		} else if (renderType == 2) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Render as triangles
		} else {
			std::cout << "Invalid render mode" << std::endl;
		}

		glBindVertexArray(VAO); // Binds VAO

		glm::mat4 modelObj = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		modelObj = glm::translate(modelObj, glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setMatrix("model", modelObj);

		// Sets render mode
		if (renderType == 0) {
			glDrawArrays(GL_POINTS, 0, model->vertices.size()); // Render as points
		} else {
			glDrawArrays(GL_TRIANGLES, 0, model->vertices.size()); // Render as lines
		}

		// Draws GUI
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		screen->drawWidgets();
		gui->refresh();

		// Swaps buffers, processes events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up remaining resources
	glfwTerminate();
	return 0;
}

void load_model(const char* pathName) {
	model->load_obj(pathName);
	camera.setNewOrigin(0, (model->maxY + model->minY)/2, 3);
	// Binds Vertex Array Object first
	glBindVertexArray(VAO);

	// Binds and sets vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * sizeof(Model::Vertex), &(model->vertices.front()), GL_STATIC_DRAW);

	// Configures vertex attributess
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (GLvoid*)offsetof(Model::Vertex, Position));
	glEnableVertexAttribArray(0);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void resetVariables(){
	cameraX = 0;
	cameraY = 0;
	cameraZ = 0;
	cameraYaw = -90;
	cameraPitch = 0;
	cameraRoll = 0;
	zNear = 0.4f;
	zFar = 5.0f;
}

// Callbacks listen for inputs.
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
	screen->keyCallbackEvent(key, scancode, action, mods);
}

void char_callback(GLFWwindow*, unsigned int codepoint) {
	screen->charCallbackEvent(codepoint);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers) {
	screen->mouseButtonCallbackEvent(button, action, modifiers);
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	screen->scrollCallbackEvent(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Resizes the window
	glViewport(0, 0, width, height);
	screen->resizeCallbackEvent(width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	screen->cursorPosCallbackEvent(xpos, ypos);
}
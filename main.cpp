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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers);
void key_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow*, unsigned int codepoint);
void load_model(const char* pathName);
void resetVariables();

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 900;

unsigned int VBO, objVAO;
Model* modelptr = nullptr;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;		// Time between current frame and last frame
float lastFrame = 0.0f;

//GUI stuff
using namespace nanogui;
Color colval(0.8f, 0.0f, 0.8f, 1.0f);
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

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // Light position will be set later
void processInput(GLFWwindow* window); //REMOVE LATER

int main() {
	// Initialize GLFW to version 3.3
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

	// Initialize GLAD
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

	// build and compile our shader program
	Shader objectShader("shader.vs", "shader.fs");
	Shader pLightShader("lighting.vs", "lighting.fs");

	// first, configure the cube's VAO (and VBO)
	glGenVertexArrays(1, &objVAO);
	glGenBuffers(1, &VBO);
	modelptr = new Model();
	load_model("resources/objects/cyborg.obj");

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Game Loop
	while (!glfwWindowShouldClose(window)) {

		// Per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input //REMOVE LATER
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE); //Activates back-face culling.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activates shaders with colors
		objectShader.use();
		objectShader.setVec3("objectColor", colval.r(), colval.g(), colval.b());
		objectShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		objectShader.setVec3("lightPos", lightPos);
		objectShader.setVec3("viewPos", camera.Position);

		// Passes projection matrix to shader
		glm::mat4 projection = glm::perspective(glm::radians(100.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, zNear, zFar);

		//Apply camera translation and rotation.
		//camera.TranslateCamera(cameraX, cameraY, cameraZ); //FIX
		//camera.RotateCamera(cameraYaw, cameraPitch, cameraRoll);
		glm::mat4 view = camera.GetViewMatrix();

		objectShader.setMat4("projection", projection);
		objectShader.setMat4("view", view);

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
		}
		else if (renderType == 1) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Render as lines
		}
		else if (renderType == 2) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Render as triangles
		}
		else {
			std::cout << "Invalid render mode" << std::endl;
		}

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		objectShader.setMat4("model", model);

		glBindVertexArray(objVAO); //Binds VAO
		glm::mat4 modelObj = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		modelObj = glm::translate(modelObj, glm::vec3(0.0f, 0.0f, 0.0f));
		objectShader.setMatrix("model", modelObj);

		// Sets render mode
		if (renderType == 0) {
			glDrawArrays(GL_POINTS, 0, modelptr->vertices.size()); // Render as points
		}
		else {
			glDrawArrays(GL_TRIANGLES, 0, modelptr->vertices.size()); // Render as lines
		}

		// Draws GUI
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		screen->drawWidgets();
		gui->refresh();

		// also draw the lamp object
		pLightShader.use();
		pLightShader.setMat4("projection", projection);
		pLightShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		pLightShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// Swaps buffers, processes events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up remaining resources
	glDeleteVertexArrays(1, &objVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void load_model(const char* pathName) {
	modelptr->loadObj(pathName);
	//camera.setNewOrigin((model->maxX + model->minX) / 2, (model->maxY + model->minY) / 2, 3);
	// Binds Vertex Array Object first
	glBindVertexArray(objVAO);

	// Binds and sets vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, modelptr->vertices.size() * sizeof(Model::Vertex), &(modelptr->vertices.front()), GL_STATIC_DRAW);

	// Configures vertex attributess
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (GLvoid*)offsetof(Model::Vertex, Position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (GLvoid*)offsetof(Model::Vertex, Position));
	glEnableVertexAttribArray(1);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void processInput(GLFWwindow* window) //REMOVE LATER
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
}

void resetVariables() {
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 0.0f;
	cameraYaw = -90;
	cameraPitch = 0;
	cameraRoll = 0;
	zNear = 0.4f;
	zFar = 5.0f;
}

// Callbacks listen for inputs.
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
	screen->resizeCallbackEvent(width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) //REMOVE LATER
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
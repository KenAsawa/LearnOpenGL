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
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;		// Time between current frame and last frame
float lastFrame = 0.0f;

//GUI stuff
using namespace nanogui;
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 0.0f;
int cameraYaw = -90;
int cameraPitch = 0;
int cameraRoll = 0;
float zNear = 0.4f;
float zFar = 15.0f;

Color objCol(1.0f, 1.0f, 1.0f, 1.0f);
int objShine = 16;
bool dLightStatus = false;
Color dLightAmbientCol(0.0f, 0.0f, 0.0f, 1.0f);
Color dLightDiffuseCol(0.0f, 0.0f, 0.0f, 1.0f);
Color dLightSpecularCol(0.0f, 0.0f, 0.0f, 1.0f);
bool pLightStatus = false;
Color pLightAmbientCol(0.0f, 0.0f, 0.0f, 1.0f);
Color pLightDiffuseCol(0.0f, 0.0f, 0.0f, 1.0f);
Color pLightSpecularCol(0.0f, 0.0f, 0.0f, 1.0f);
bool pLightRotateX = false;
bool pLightRotateY = false;
bool pLightRotateZ = false;

enum test_enum {
	Item1,
	Item2,
	Item3
};
test_enum renderType = test_enum::Item3;
test_enum cullingType = test_enum::Item2;
test_enum shadingType = test_enum::Item2;
test_enum depthType = test_enum::Item2;
std::string modelName = "cyborg.obj";

Screen* screen = nullptr;

// Lighting
glm::vec3 pLightPos(1.2f, 1.0f, 2.0f); // Light position will be set later
glm::vec3 dLightDir(0.0f, -1.0f, -1.0f); // Directional light direction

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
	ref<Window> nanoguiWindow = 
	// First nanogui gui
	gui->addWindow(Eigen::Vector2i(10, 10), "Control Bar 1");
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
	gui->addVariable("Shading Type", shadingType, enabled)->setItems({ "FLAT", "SMOOTH" });
	gui->addVariable("Depth Type", depthType, enabled)->setItems({ "LESS", "ALWAYS" });
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

	// Second nanogui gui
	gui->addWindow(Eigen::Vector2i(210, 10), "Control Bar 2");
	gui->addGroup("Lighting");
	gui->addVariable("Object Color:", objCol);
	gui->addVariable("Object Shininess", objShine);
	gui->addVariable("Direction Light Status", dLightStatus);
	gui->addVariable("Direction Light Ambient Color", dLightAmbientCol);
	gui->addVariable("Direction Light Diffuse Color", dLightDiffuseCol);
	gui->addVariable("Direction Light Specular Color", dLightSpecularCol);
	gui->addVariable("Point Light Status", pLightStatus);
	gui->addVariable("Point Light Ambient Color", pLightAmbientCol);
	gui->addVariable("Point Light Diffuse Color", pLightDiffuseCol);
	gui->addVariable("Point Light Specular Color", pLightSpecularCol);
	gui->addVariable("Point Light Rotate on X", pLightRotateX);
	gui->addVariable("Point Light Rotate on Y", pLightRotateY);
	gui->addVariable("Point Light Rotate on Z", pLightRotateZ);
	screen->setVisible(true);
	screen->performLayout();
	//End of nanogui gui

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();
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

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST); // Enables depth test
		// Sets depth test type
		if (depthType == 1) {
			glDepthFunc(GL_LESS);
		}
		else {
			glDepthFunc(GL_ALWAYS);
		}
		glEnable(GL_CULL_FACE); //Activates back-face culling.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//float radius = (modelptr->maxX + modelptr->minX) / 2;
		float radius = 5;
		float lightPosX = sin(glfwGetTime()) * radius;
		float lightPosY = cos(glfwGetTime()) * radius;
		//float lightPosZ = cos(glfwGetTime()) * radius;
		//pLightPos = glm::vec3(lightPosX, lightPosY, lightPosZ); //Sets point light position
		pLightPos = glm::vec3((modelptr->maxX + modelptr->minX)/2 + lightPosX, modelptr->maxY + lightPosY, modelptr->maxZ + 1); //Sets point light position

		// Activates shaders with colors
		objectShader.use();
		if (shadingType == 0) {
			objectShader.setBool("applySmoothing", false);
		}
		else if (shadingType == 1) {
			objectShader.setBool("applySmoothing", true);
		}

		objectShader.setVec3("objectColor", objCol.r(), objCol.g(), objCol.b());
		objectShader.setInt("objectShine", objShine);
		objectShader.setVec3("viewPos", camera.Position);

		if (pLightStatus) {
			objectShader.setVec3("pointLight.position", pLightPos);
			objectShader.setVec3("pointLight.ambientLightColor", pLightAmbientCol.r(), pLightAmbientCol.g(), pLightAmbientCol.b());
			objectShader.setVec3("pointLight.diffuseLightColor", pLightDiffuseCol.r(), pLightDiffuseCol.g(), pLightDiffuseCol.b());
			objectShader.setVec3("pointLight.specularLightColor", pLightSpecularCol.r(), pLightSpecularCol.g(), pLightSpecularCol.b());
		}
		else {
			objectShader.setVec3("pointLight.ambientLightColor", 0,0,0);
			objectShader.setVec3("pointLight.diffuseLightColor", 0, 0, 0);
			objectShader.setVec3("pointLight.specularLightColor", 0, 0, 0);
		}
		if (dLightStatus) {
			objectShader.setVec3("dirLight.direction", dLightDir);
			objectShader.setVec3("dirLight.ambientLightColor", dLightAmbientCol.r(), dLightAmbientCol.g(), dLightAmbientCol.b());
			objectShader.setVec3("dirLight.diffuseLightColor", dLightDiffuseCol.r(), dLightDiffuseCol.g(), dLightDiffuseCol.b());
			objectShader.setVec3("dirLight.specularLightColor", dLightSpecularCol.r(), dLightSpecularCol.g(), dLightSpecularCol.b());
		}
		else {
			objectShader.setVec3("dirLight.ambientLightColor", 0, 0, 0);
			objectShader.setVec3("dirLight.diffuseLightColor", 0, 0, 0);
			objectShader.setVec3("dirLight.specularLightColor", 0, 0, 0);
		}

		//Apply camera translation and rotation.
		camera.TranslateCamera(cameraX, cameraY, cameraZ);
		camera.RotateCamera(cameraYaw, cameraPitch, cameraRoll);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, zNear, zFar);
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

		// Render's the light.
		pLightShader.use();
		pLightShader.setMat4("projection", projection);
		pLightShader.setMat4("view", view);

		// Draws GUI
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		screen->drawWidgets();
		gui->refresh();

		// Swaps buffers, processes events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up remaining resources
	glDeleteVertexArrays(1, &objVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void load_model(const char* pathName) {
	modelptr->loadObj(pathName);
	camera.setNewOrigin((modelptr->maxX + modelptr->minX) / 2, (modelptr->maxY + modelptr->minY) / 2, 6);
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

void resetVariables() {
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 0.0f;
	cameraYaw = -90;
	cameraPitch = 0;
	cameraRoll = 0;
	zNear = 0.4f;
	zFar = 15.0f;
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
	screen->cursorPosCallbackEvent(xpos, ypos);
}
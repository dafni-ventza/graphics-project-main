// Include C++ headers
#include <iostream>
#include <string>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include "model.h"
#include "texture.h"
#include "FountainEmitter.h"
#include "OrbitEmitter.h"
#include "common/Random.h"


//TODO delete the includes afterwards
#include <chrono>
using namespace std::chrono;
//

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void windXManipulation();
void windZManipulation();

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Rain Apallaktiki"

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint particleShaderProgram, normalShaderProgram, terrainShaderProgram;
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation, projectionAndViewMatrix;
GLuint translationMatrixLocation, rotationMatrixLocation, scaleMatrixLocation;
GLuint sceneTexture, waterSampler, waterTexture, sceneSampler, cloudTexture, cloudSampler;
GLuint lightLocation;

particleAttributes particle;

//Lighting for terrain
GLfloat g_LighDir[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat g_LightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat g_LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat g_LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat g_LighAttenuation0 = 1.0f;
GLfloat g_LighAttenuation1 = 0.0f;
GLfloat g_LighAttenuation2 = 0.0f;



// light properties
GLuint LaLocation, LdLocation, LsLocation, lightPositionLocation, lightPowerLocation;
// material properties
GLuint KdLocation, KsLocation, KaLocation, NsLocation;

glm::vec3 slider_emitter_pos(0.0f, 0.0f, 0.0f);
//Particles in the beginning...
int particles_slider = 5;


void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

bool game_paused = false;

bool use_sorting = false;
bool use_rotations = false;

float height_threshold = W_HEIGHT / 2.0f;
//float height_threshold = 100.0f;

//Model Scene Load
GLuint modelVAO, modelVerticiesVBO, planeVAO, planeVerticiesVBO;
//vector<list<edge*>> graph;
std::vector<vec3> modelVertices, modelNormals(8000);
//Fixing the exception errors of sizeof(glm::vec3) by resizing the graph vector before pushing, or simpler giveing initial size by
std::vector<vec2> modelUVs; //https://stackoverflow.com/questions/61783830/problem-in-accessing-vectors-of-list-in-c
GLuint MVPLocation, MLocation;

GLuint modelUVVBO, modelNormalsVBO;


#define gg 9.80665f

struct Light {
	glm::vec4 La;
	glm::vec4 Ld;
	glm::vec4 Ls;
	glm::vec3 lightPosition_worldspace;
	float power;
};

struct Material {
	glm::vec4 Ka;
	glm::vec4 Kd;
	glm::vec4 Ks;
	float Ns;
};

const Material myMaterial{
	vec4{ 0.1, 0.1, 0.1, 1 },
	vec4{ 1.0, 1.0, 1.0, 1 },
	vec4{ 0.3, 0.3, 0.3, 1 },
	0.1f
};

Light light{
	vec4{ 1, 1, 1, 1 },
	vec4{ 1, 1, 1, 1 },
	vec4{ 1, 1, 1, 1 },
	vec3{ 0, 4, 4 },
	20.0f
};

glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.1f, 0.2f);


void renderHelpingWindow() {
	static int counter = 0;

	ImGui::Begin("Helper Window");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("Use arrow keys for zoom in/out and A,W,S,D for moving around world space. Press ESC for exit.\n");               // Display some text (you can use a format strings too)

	ImGui::ColorEdit3("Background", &background_color[0]);

	ImGui::SliderFloat("x position", &slider_emitter_pos[0], -30.0f, 30.0f);
	ImGui::SliderFloat("y position", &slider_emitter_pos[1], -30.0f, 30.0f);
	ImGui::SliderFloat("z position", &slider_emitter_pos[2], -30.0f, 30.0f);
	ImGui::SliderFloat("height", &height_threshold, 0, 500);

	ImGui::SliderInt("particles", &particles_slider, 0, 20000);


	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Checkbox("Use sorting", &use_sorting);
	ImGui::Checkbox("Use rotations", &use_rotations);

	ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void createContext() {
	particleShaderProgram = loadShaders(
		"ParticleShader.vertexshader",
		"ParticleShader.fragmentshader");

	normalShaderProgram = loadShaders(
		"StandardShading.vertexshader",
		"StandardShading.fragmentshader");


	projectionAndViewMatrix = glGetUniformLocation(particleShaderProgram, "PV");

	translationMatrixLocation = glGetUniformLocation(normalShaderProgram, "T");
	rotationMatrixLocation = glGetUniformLocation(normalShaderProgram, "R");
	scaleMatrixLocation = glGetUniformLocation(normalShaderProgram, "S");

	modelMatrixLocation = glGetUniformLocation(normalShaderProgram, "M");
	viewMatrixLocation = glGetUniformLocation(normalShaderProgram, "V");
	projectionMatrixLocation = glGetUniformLocation(normalShaderProgram, "P");
	lightLocation = glGetUniformLocation(normalShaderProgram, "light_position_worldspace");

	//// MODEL STUFF
	// Get a pointer location to model matrix in the vertex shader
	MVPLocation = glGetUniformLocation(normalShaderProgram, "MVP");

	waterSampler = glGetUniformLocation(particleShaderProgram, "texture0");
	//Attribution for water texture: <a href='https://www.freepik.com/photos/water'>Water photo created by rawpixel.com - www.freepik.com</a>
	waterTexture = loadSOIL("water.png");


	cloudSampler = glGetUniformLocation(particleShaderProgram, "texture2");
	cloudTexture = loadSOIL("cloud.png");


	sceneSampler = glGetUniformLocation(normalShaderProgram, "texture1");
	sceneTexture = loadSOIL("height-map-vikos-mountain.png");
	//

	loadOBJWithTiny("vikos-terrain.obj",
		modelVertices,
		modelUVs,
		modelNormals);

	
	// Draw wire frame triangles or fill: GL_LINE, or GL_FILL
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//VAO
	glGenVertexArrays(1, &modelVAO);
	glBindVertexArray(modelVAO);


	//VBO
	glGenBuffers(1, &modelVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(glm::vec3), //modelVertices std::vector<glm::vec3>
		&modelVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glfwMakeContextCurrent(window); //checking for multithreading issues


	// Computing the normals
	glGenBuffers(1, &modelNormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, modelNormals.size() * sizeof(glm::vec3),
		&modelNormals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// uvs VBO
	glGenBuffers(1, &modelUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelUVVBO);
	glBufferData(GL_ARRAY_BUFFER, modelUVs.size() * sizeof(glm::vec2),
		&modelUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glfwSetKeyCallback(window, pollKeyboard);


}

void free() {

	glDeleteBuffers(1, &modelVerticiesVBO);
	glDeleteBuffers(1, &modelUVVBO);
	glDeleteBuffers(1, &modelNormalsVBO);
	glDeleteVertexArrays(1, &modelVAO);

	glDeleteTextures(1, &waterTexture);
	glDeleteTextures(1, &cloudTexture);
	glDeleteTextures(1, &sceneTexture);

	glDeleteProgram(particleShaderProgram);
	glDeleteProgram(normalShaderProgram);

	glfwTerminate();
}

void mainLoop() {



	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	double t = glfwGetTime();
	vec3 lightPos = vec3(10, 10, 10);


	//Find a more realistic obj in future
	auto* sphere = new Drawable("droplet.obj");

	FountainEmitter f_emitter = FountainEmitter(sphere, particles_slider);

	auto* cloud = new Drawable("droplet.obj");
	OrbitEmitter cloud_emitter = OrbitEmitter(cloud, 100, 0, 1);
	//FountainEmitter cloud_emitter = FountainEmitter(cloud, particles_slider);


	do {

		double currentTime = glfwGetTime();
		double dt = currentTime - t;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glMatrixMode(GL_PROJECTION);

		//camera
		camera->update();
		mat4 projectionMatrix, viewMatrix;

		////Model Loading...
		glUseProgram(normalShaderProgram);

		

		glBindVertexArray(modelVAO);

		auto modelMatrix = mat4(1);
		projectionMatrix = camera->projectionMatrix;
		viewMatrix = camera->viewMatrix;


		glMatrixMode(GL_MODELVIEW);

		auto modelTranslation = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	
		float scaleFactor = 0.01f;
		//float scaleFactor = 50.0f;
		//mat4 modelScale = glm::scale(modelTranslation, glm::vec3(scaleFactor, 1.0f, 1.0f));
		auto modelScale = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		//auto modelRotate = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		//Cumulating the transformations
		auto transformedModelMatrix = modelTranslation * modelScale * modelMatrix;
		//
		//auto modelMVP = projectionMatrix * viewMatrix * modelTranslation * modelRotate * modelScale * modelMatrix;
		auto modelMVP = projectionMatrix * viewMatrix * modelMatrix;

		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &modelMVP[0][0]);
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); //model new
		glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &modelTranslation[0][0]);
		glUniformMatrix4fv(scaleMatrixLocation, 1, GL_FALSE, &modelScale[0][0]);
		//glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &modelRotate[0][0]);

		// light
		auto lightPos_cameraspace = vec3(viewMatrix * modelScale* vec4(lightPos, 1.0f));
		glUniform3f(lightLocation, lightPos_cameraspace.x, lightPos_cameraspace.y, lightPos_cameraspace.z);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneTexture);
		glUniform1i(sceneSampler, 0);



		glDrawArrays(GL_TRIANGLES, 0, modelVertices.size());
		//glDrawElements(GL_TRIANGLES, modelVertices.size(), GL_UNSIGNED_BYTE, NULL);
		//glDrawElements(GL_TRIANGLES, modelVertices.size(), GL_UNSIGNED_INT, &modelVertices[0]);

		/*cout << "Model pos:" << modelM << endl;*/

		f_emitter.changeParticleNumber(particles_slider);
		f_emitter.emitter_pos = slider_emitter_pos;
		f_emitter.use_rotations = use_rotations;
		f_emitter.use_sorting = use_sorting;
		f_emitter.height_threshold = height_threshold;


		glUseProgram(particleShaderProgram);

		// camera
		camera->update();
		
		projectionMatrix = camera->projectionMatrix;
		viewMatrix = camera->viewMatrix;

		auto PV = projectionMatrix * viewMatrix;
		glUniformMatrix4fv(projectionAndViewMatrix, 1, GL_FALSE, &PV[0][0]);

		//*/ Use particle based drawing
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterTexture);
		glUniform1i(waterSampler, 0);
		if (!game_paused) {
			f_emitter.updateParticles(currentTime, dt, camera->position);
			cloud_emitter.updateParticles(currentTime, dt, camera->position);
		}

		//Particles draw
		f_emitter.renderParticles();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cloudTexture);
		glUniform1i(cloudSampler, 0);
		cloud_emitter.renderParticles();

		//After rendering the first object - insert collision function
		//handleBoxSphereCollision(*box, *sphere);

	
	
		

		renderHelpingWindow();
		glfwPollEvents();
		glfwSwapBuffers(window);
		t = currentTime;

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


}

void initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW\n");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_error(string(string("Failed to open GLFW window.") +
			" If you have an Intel GPU, they are not 3.3 compatible." +
			"Try the 2.1 version.\n"));
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Start GLEW extension handler
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_error("Failed to initialize GLEW\n");
	}

		
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

	// Reset the background
	glClearColor(0.0f, 0.0f, 0.1f, 0.2f);

	glfwSetKeyCallback(window, pollKeyboard);

	glEnable(GL_TRANSFORM_BIT);


	
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_MODELVIEW);
	//glDepthFunc(GL_LESS);  //function used to compare each incoming pixel depth value with the depth value present in the depth buffer


	////Terrain stuff until glColorMaterial
	//glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	////glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	////gluBuild2DMipmaps(GL_TEXTURE_2D, 0, W_WIDTH, W_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	//
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_LightAmbient);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, g_LightSpecular);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, g_LighAttenuation0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, g_LighAttenuation1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, g_LighAttenuation2);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE); //In order to cull polygons based on their winding in window coordinates
	glDisable(GL_CULL_FACE);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
							//by default CCW
	//glCullFace(GL_BACK); //If mode is GL_FRONT_AND_BACK, no polygons are drawn, but other primitives such as points and lines are drawn.
	//glFrontFace(GL_CCW); //entirely of opaque closed surfaces, back-facing polygons are never visible

	
	// set the texture wrapping parameters
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	// set texture filtering parameters
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
	//glEnable(GL_BLEND); //In order to the computed fragment color values with the values in the color buffers.  - FOR TRANSPARENT OBJ
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//// enable point size when drawing points
	glEnable(GL_PROGRAM_POINT_SIZE);


	// Log
	logGLParameters();

	// Create camera
	camera = new Camera(window);

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		camera->onMouseMove(xpos, ypos);
	}
	);

}

void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Pause
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		game_paused = !game_paused;
	}

	//Less droplets - L
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		particle.life--;
		particles_slider--;
	}
	//More droplets - M
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		particle.life++;
		particles_slider++;
	}

	//Add wind in X Axis - I
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		windXManipulation();
		//std::cout << "Wind in X direction applied of force predefined" << std::endl;

	}
	//More Wind in Z Axis - U
	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		windZManipulation();
		//std::cout << "Wind in Z direction applied of force predefined" << std::endl;

	}


	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			camera->active = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			camera->active = false;
		}

	}
}

int main(void) {
	try {
		initialize();
		createContext();



		mainLoop();
		free();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		getchar();
		free();
		return -1;
	}

	return 0;
}


void windXManipulation() {
	particle.factorXWind += particle.factorXWind;

	particle.velocity = glm::vec3(0.5 - RAND * particle.factorXWind,
		-particle.speedYDroplet,
		5 - RAND * particle.factorXWind)*particle.speedDropFall;

	/*std::cout << particle.velocity << std::endl;*/
	//std::cout << "Velocity - I: " << glm::to_string(particle.velocity) << std::endl;
	//std::cout << "Particle factorXWind - I: " << (particle.factorXWind) << std::endl;
	std::cout << "Wind in X direction applied of force:"<< particle.factorXWind << std::endl;
}

void windZManipulation() {
	particle.factorZWind += particle.factorZWind;

	particle.velocity = glm::vec3(
		100* Random::Float() *particle.factorZWind,
		-particle.speedYDroplet,
		5 - RAND * particle.factorZWind)*particle.speedDropFall;



	particle.rot_axis = glm::normalize(glm::vec3(
	0,
	1 - 2 * RAND,
	0));

	std::cout << "Wind in Z direction applied of force:" << particle.factorZWind << std::endl;
	///*std::cout << particle.velocity << std::endl;*/
	//std::cout << "Velocity - U: " << glm::to_string(particle.velocity) << std::endl;
	//std::cout << "Particle factorXWind - U: " << (particle.factorZWind) << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
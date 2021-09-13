#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <iostream>



using namespace glm;

Camera::Camera(GLFWwindow* window) : window(window) {
	//position = glm::vec3(0, 0, 5);
	position = glm::vec3(0, 0, 40);
	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;
	FoV = 67.0f;
	speed = 3.0f;
	mouseSpeed = 0.001f;
	fovSpeed = 2.0f;
}

void Camera::update() {
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	int width, height;

	float near = 0.001f; // clipping plane
	float far = 5400.0f; // clipping plane
	float aspect = 4 / 3.0f; // aspect ratio

	// matrix components
	float range = tan(FoV * 0.5f) * near;
	float Sx = (2.0f * near) / (range * aspect + range * aspect);
	float Sy = near / range;
	float Sz = -(far + near) / (far - near);
	float Pz = -(2.0f * far * near) / (far - near);

	projectionMatrix = {
	Sx, 0.0f, 0.0f, 0.0f,
	0.0f, Sy, 0.0f, 0.0f,
	0.0f, 0.0f, Sz, -1.0f,
	0.0f, 0.0f, Pz, 0.0f
	};


	// Get mouse position
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);


	glfwGetWindowSize(window, &width, &height);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, width / 2, height / 2);

	// Task 5.1: simple camera movement that moves in +-z and +-x axes

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position -= vec3(0, 0, 1) * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position += vec3(0, 0, 1) * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += vec3(1, 0, 0) * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= vec3(1, 0, 0) * deltaTime * speed;
	}

	//Task 5.2: update view matrix so it always looks at the origin

	//projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 1000.0f, 5000.0f);

	projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, near, far);
	/*projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 10.0f, 100.0f);
	projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 1000000.0f);

	projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.001f, 9000.0f);*/
	//projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	viewMatrix = lookAt(
		position,
		vec3(0, 1, 0),
		vec3(0, 0, 1)
	);


	// Task 5.3: Compute new horizontal and vertical angles, given windows size
	//*/
	// and cursor position
	//horizontalAngle += mouseSpeed * float(width / 2 - xPos);
	//verticalAngle += mouseSpeed * float(height / 2 - yPos);

	horizontalAngle += mouseSpeed * (xPos - (width / 2.0f));
	verticalAngle += mouseSpeed * (yPos - (height / 2.0f));


	// Task 5.4: right and up vectors of the camera coordinate system
	// use spherical coordinates
	vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	vec3 right(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	vec3 up = cross(right, direction);

	// Task 5.5: update camera position using the direction/right vectors
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		position -= up * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		position += up * deltaTime * speed;
	}


	// Task 5.6: handle zoom in/out effects
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		FoV -= fovSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		FoV += fovSpeed;
	}


	// Task 5.7: construct projection and view matrices

	projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, near, far);

	//projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.0005f, 500000.0f);
   //projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 1000000.0f);
   //projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 800.0f);

	viewMatrix = lookAt(
		position,
		position + direction,
		up
	);
	//*/





	// Homework XX: perform orthographic projection
	//glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
	//glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;


}


void Camera::onMouseMove(double xPos, double yPos) {
	static double lastxPos = xPos;
	static double lastyPos = yPos;

	horizontalAngle += mouseSpeed * (lastxPos - xPos);
	verticalAngle += mouseSpeed * (lastyPos - yPos);

	lastxPos = xPos;
	lastyPos = yPos;
}
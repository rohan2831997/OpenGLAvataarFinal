#include"camera.h"

Camera::Camera(const int width, const int height, vec3 position)
{
	this->width = width;
	this->height = height;
	this->position = position;
	this->view = lookAt(position, position + orientation, up);
	this->proj = perspective(radians(45.0f), float(width / height), 0.1f, 100.0f);
}

void Camera::UpdateMatrix(float FOVdegrees, float NearPlane, float FarPlane)
{
	//define the matrices
	//assign values to the matrices
	view = lookAt(position, position + orientation, up);
	proj = perspective(radians(FOVdegrees), float(width / height), NearPlane, FarPlane);
	cameraMatrix = proj * view;
}

void Camera::SendMatrixToShader(Shader& shader, const char* uniform)
{
	//get the values of the matrix unoforms
	int cammatrixloc = glGetUniformLocation(shader.ID, uniform);
	glUniformMatrix4fv(cammatrixloc, 1, GL_FALSE, value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window, vector<vector<mat4>>& sphereModel3, vector < vector<vec3> >& colorMod)
{
	// Handles key inputs
	//close the window on pressing escape
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += speed * -glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += speed * glm::normalize(glm::cross(orientation, up));
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		detectSpheres(window, sphereModel3, colorMod);
	}

	// Handles mouse inputs
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees
		// rotX means rotation about x-axis
		// rotY means rotation about y-axis
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the orientation
		glm::vec3 neworientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Decides whether or not the next vertical orientation is legal or not
		if (abs(glm::angle(neworientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = neworientation;
		}

		// Rotates the orientation left and right
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}

}

double Camera::distance(float cx, float cy, float cz, float tx, float ty, float tz)
{
	return sqrt(pow(cx - tx, 2) + pow(cy - ty, 2) + pow(cz - tz, 2));
}

void Camera::detectSpheres(GLFWwindow* window, vector<vector<mat4>> sphereModel3, vector < vector<vec3>>& colorMod)
{
	int viewport[4];
	vec3 nearLoc, farLoc; //declared for far points
	glGetIntegerv(GL_VIEWPORT, viewport);
	ivec4 Viewport = { viewport[0],viewport[1] ,viewport[2] ,viewport[3] };

	// Stores the coordinates of the cursor
	double mouseX;
	double mouseY;
	// Fetches the coordinates of the cursor
	glfwGetCursorPos(window, &mouseX, &mouseY);

	nearLoc = unProject(vec3(mouseX, viewport[3] - mouseY, 0.0f), view, proj, Viewport);
	farLoc = unProject(vec3(mouseX, viewport[3] - mouseY, 1.0f), view, proj, Viewport);

	//Ray walking code
	float tx, ty, tz;

	//iterate through all the spheres
	for (float x = 0; x < 6; x++)
	{
		for (float y = 0; y < 6; y++)
		{
			//ray walking logic
			//check if any point is less than 1 radius = 0.1f distance
			for (float t = 0.018f; t <= 0.020f; t += 0.0001f) {
				vec4 spherePos = sphereModel3[x][y] * (vec4(0.0f, 0.0f, 0.0f, 1.0f));
				tx = nearLoc.x + t * (farLoc.x - nearLoc.x);
				ty = nearLoc.y + t * (farLoc.y - nearLoc.y);
				tz = nearLoc.z + t * (farLoc.z - nearLoc.z);

				if (distance(spherePos.x, spherePos.y, spherePos.z, tx, ty, tz) <= 0.1f)
				{
					//to know which sphere was clicked
					// todebug remove the comments
					//cout << "SPHERE " << x << " " << y << " CLICKED" << endl;
					

					//now change colour of the sphere
					//radius of the spheres is 0.1f hence appropriate scaling factor so that max value of normal
					//could be (1,1,1)
					float scale = 10.0f * sqrt(3.0f);

					vec3 normal(tx - spherePos.x, (ty - spherePos.y), (tz - spherePos.z));
					normal = normal * scale;
					colorMod[x][y] = (normal + vec3(1.0f, 1.0f, 1.0f)) * 0.5f;

					//The further program is about resetting the colorMod if we did not click on sphere
					return;
				}
			}
		}
	}

	//reset if not clicked on speres
	fill(colorMod.begin(), colorMod.end(), vector<vec3>(6, vec3(1.0f, 1.0f, 1.0f)));
}

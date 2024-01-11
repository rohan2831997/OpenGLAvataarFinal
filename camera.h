#ifndef CAMERA_CLASS
#define CAMERA_CLASS

#include<vector>
#include<algorithm>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <cmath>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
//#include<GL/GLU.h>

#include"shaderClass.h"

using namespace glm;

class Camera
{
public:
	int width;
	int height;

	vec3 position;
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 orientation = vec3(0.0f, 0.0f, -1.0f);
	mat4 cameraMatrix = glm::mat4(1.0f);
	mat4 view;
	mat4 proj;

	Camera(const int width, const int height, vec3 position);
	void UpdateMatrix(float FOVdegrees, float NearPlane, float FarPlane);
	void SendMatrixToShader(Shader& shader, const char* uniform);

	void Inputs(GLFWwindow* window, vector<vector<mat4>>& sphereModel3, vector<vector<vec3>>& colorMod);

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.001f;
	float sensitivity = 100.0f;

	bool firstClick = true;

	//detectspheres
	void detectSpheres(GLFWwindow* window, vector<vector<mat4>> sphereModel3, vector < vector<vec3> >& colorMod);

	double distance(float cx, float cy, float cz, float tx, float ty, float tz);


};


#endif // !CAMERA_CLASS

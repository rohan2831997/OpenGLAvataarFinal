#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <cmath>
#include<stb/stb_image.h.txt>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Texture.h"
#include"camera.h"
#include"mesh.h"

using namespace glm;



// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-5.0f,  5.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-5.0f, -5.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(5.0f, -5.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(5.0f,  5.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

void SphereVerticeGenerate(float radius, int sectorCount, int stackCount, vector<Vertex>& verticesF)
{

	float PI = 3.14f;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			verticesF.push_back(Vertex{ vec3(x, y, z),vec3(nx, ny, nz), glm::vec3(0.0f, 1.0f, 0.0f), vec2(s, t) });
		}
	}
};

std::vector<GLuint> IndicesGenerator(int stackCount, int sectorCount)
{
	// generate CCW index list of sphere triangles
// k1--k1+1
// |  / |
// | /  |
// k2--k2+1
	std::vector<GLuint> indices;
	std::vector<GLuint> lineIndices;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}

			// store indices for lines
			// vertical lines for all stacks, k1 => k2
			lineIndices.push_back(k1);
			lineIndices.push_back(k2);
			if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
			{
				lineIndices.push_back(k1);
				lineIndices.push_back(k1 + 1);
			}
		}
	}

	return indices;
}



const unsigned int width = 800;
const unsigned int height = 800;

int main()
{

	std::vector<vec3> Vertices;
	std::vector<vec3> Normals;
	std::vector<vec2> TexCoords;


	vector<Vertex> verticesF;
	SphereVerticeGenerate(0.1f, 12, 12, verticesF);

	vector<GLuint> indicesF = IndicesGenerator(12, 12);

	//Initialise GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using
	// In this case openGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Tell GLFW we are using the CORE profile
	//So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "FAILED to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load Glad so that it configures OPENGL
	gladLoadGL();

	// Specify the view port in the window
	// In this case the viewport goes from x=0, y=0, to x=800, y=800
	glViewport(0, 0, width, height);

	Texture textures[]
	{
			Texture("planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("planksspec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	Texture texturesback[]
	{
			Texture("BackGroundFill.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
			Texture("BackgroundFillSpecularMap.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};


	//create the shader
	Shader shaderProgram("default.vert", "default.frag");
	std::vector<Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	std::vector<Texture> texBack(texturesback, texturesback + sizeof(texturesback) / sizeof(Texture));

	//Mesh of Background
	Mesh background(verts, ind, texBack);

	Mesh sphere(verticesF, indicesF, tex);

	//create the light object
	//create the light shader
	Shader lightShader("light.vert", "light.frag");

	std::vector<Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector<GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));

	Mesh light(lightVerts, lightInd, tex);

	//make the model matrix for light
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec3 lightPos = vec3(0.0f, 1.5f, 1.5f);
	mat4 lightModel = mat4(1.0f);
	lightModel = translate(lightModel, lightPos);

	//make the model matrix for sphere
	float scale = 0.6f;
	vector<vector<mat4>> sphereModel(6, vector<mat4>(6, mat4(1.0f)));

	//make all the sphers
	//To change colors of sphere
	vector<vector<vec3>>colorMod(6, vector<vec3>(6, vec3(1.0f, 1.0f, 1.0f)));
	for (float x = 0; x < 6; x++)
	{
		for (float y = 0; y < 6; y++)
		{
			vec3 spherePos = vec3((-1.2f + (x / 2.1f)) * scale, 0.69f - (y / 2.1f) * scale, -0.0f);
			sphereModel[x][y] = translate(sphereModel[x][y], spherePos);
		}
	}

	//model for background
	vec3 backPos = vec3(+0.0f, 0.0f, -3.0f);
	mat4 backModel = mat4(1.0f);
	backModel = translate(backModel, backPos);

	//input the respective values of unifrom from shaders
	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

	shaderProgram.Activate();
	//glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(sphereModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	//Specify the background collour
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	// Clean the back buffer and assign the new colour to it
	glClear(GL_COLOR_BUFFER_BIT);
	//swap the back buffer witht he front buffer
	glfwSwapBuffers(window);

	//enable depth filter
	glEnable(GL_DEPTH_TEST);

	//Camera class
	Camera cam(width, height, vec3(0.0f, 0.0f, 2.0f));

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new colour to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cam.Inputs(window, sphereModel, colorMod);
		cam.UpdateMatrix(45.0f, 0.1f, 100.0f);

		//handle Uniforms
		//Linear diffuse and specular lighting
		for (float x = 0; x < 6; x++)
		{
			for (float y = 0; y < 6; y++)
			{
				glUniform1f(glGetUniformLocation(shaderProgram.ID, "diffuseFactor"), 1.0f - x / 6.0f);
				glUniform1f(glGetUniformLocation(shaderProgram.ID, "specFactor"), 1.0f - y / 6.0f);
				sphere.Draw(shaderProgram, cam, sphereModel[x][y], colorMod[x][y]);
			}
		}

		//to draw light object properly
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "diffuseFactor"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "specFactor"), 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "colorMod"), 1.0f, 1.0f, 1.0f);

		//background.Draw(shaderProgram, cam, backModel);

		light.Draw(lightShader, cam, lightModel);

		glfwSwapBuffers(window);
		//take care of all GLFW events
		glfwPollEvents();
	}


	// Delete all the objects we've created

	shaderProgram.Delete();
	lightShader.Delete();

	// Delete window after use
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>
#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"VAO.h"
#include"EBO.h"
#include"camera.h"
#include"Texture.h"

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	VAO VAO;

	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture> textures);

	void Draw(Shader& shader, Camera& camera, 
		vec3 colorMod = vec3(1.0f,1.0f,1.0f), 
		glm::mat4 model = glm::mat4(1.0f),
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

	void DrawLight(Shader& shader, Camera& camera, glm::mat4& model);
};


#endif // !MESH_CLASS_H

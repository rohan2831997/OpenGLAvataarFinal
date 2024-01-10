#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>
#include<vector>

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

	void Draw(Shader& shader, Camera& camera, glm::mat4& model, vec3& colorMod);
	void Draw(Shader& shader, Camera& camera, glm::mat4& model);
};


#endif // !MESH_CLASS_H

#ifndef MODEL_H
#define MODEL_H


#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Model {
public:
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};
	Model() {
		std::vector<Vertex> vertices;
	}
	std::vector<Vertex> vertices;
	float minX = 0.0f;
	float maxX = 0.0f;
	float minY = 0.0f;
	float maxY = 0.0f;
	float minZ = 0.0f;
	float maxZ = 0.0f;
	void loadObj(std::string pathName);
};

#endif
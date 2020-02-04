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
		// Position
		glm::vec3 Position;
		// Normal
		glm::vec3 Normal;
		// TexCoords
		glm::vec2 TexCoords;
	};
	Model() {
		std::vector<Vertex> vertices;
	}
	std::vector<Vertex> vertices;
	void load_obj(std::string obj_path);
};

#endif
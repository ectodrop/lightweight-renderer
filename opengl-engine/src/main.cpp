#include "pch.h"

#include "engine.h"

//unsigned int loadCubemap(std::vector<std::string> faces) {
//	stbi_set_flip_vertically_on_load(false);
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//	int width, height, nrChannels;
//	for (int i = 0; i < 6; i++) {
//		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//		if (data) {
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		}
//		else {
//			std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
//		}
//		stbi_image_free(data);
//	}
//
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	return textureID;
//}

std::vector<float> genTorus(float inner, float outer, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
	int slices = 40;
	int sliceVertices = 40;
	float thickness = outer - inner;
	float center = outer - (thickness / 2);
	for (int slice = 0; slice < slices; slice++) {
		float yaw = slice * ((M_PI * 2.0f) / (float)slices);
		glm::vec3 offset(center * cos(yaw), 0.0f, center * sin(yaw));
		for (int vertex = 0; vertex < sliceVertices; vertex++) {
			float pitch = vertex * ((M_PI * 2.0f) / (float)sliceVertices);
			glm::vec3 circle = glm::vec3(
				thickness * cos(yaw) * cos(pitch),
				thickness * sin(pitch),
				thickness * sin(yaw) * cos(pitch)
			);
			// std::cout << (int)circle.x << "," << (int)circle.y << "," << (int)circle.z << std::endl;
			circle += offset;
			// insert vertex position
			vertices.insert(vertices.end(), { circle.x, circle.y, circle.z });
			circle -= offset;
			// insert vertex normal
			vertices.insert(vertices.end(), { circle.x, circle.y, circle.z });
		}
	}
	int vertexSize = 6;
	int rightOffset = sliceVertices;
	int numVertices = slices * sliceVertices;
	for (int i = 0; i < numVertices; i++) {
		int offset = 1;
		if ((i + 1) % sliceVertices == 0) offset += -1 * sliceVertices;
		indices.push_back(i);
		indices.push_back((i + rightOffset) % numVertices);
		indices.push_back((i + rightOffset + offset) % numVertices);

		indices.push_back(i);
		indices.push_back((i + offset) % numVertices);
		indices.push_back((i + rightOffset + offset) % numVertices);
		//std::cout << i << "," << (i+rightOffset) % numVertices<< "," << (i+rightOffset+offset) % numVertices<<std::endl;
		//std::cout << i << "," << (i+offset)  % numVertices<< "," << (i+rightOffset+offset) % numVertices<<std::endl;
	}
	//std::cout << vertices.size() << std::endl;
	return vertices;
}

int main()
{
	OpenGLEngine* engine = new OpenGLEngine();
	while (!engine->ShouldClose())
	{
		engine->Update();
	}
	
	return 0;
}

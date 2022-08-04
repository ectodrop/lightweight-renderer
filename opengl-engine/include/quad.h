#ifndef QUAD_H
#define QUAD_H
#include "pch.h"
#include <shader.h>

class Quad {
private:
	unsigned int VAO, VBO;
	Shader quadShader = Shader("shaders\\quad.vert", "shaders\\quad.frag");
public:
	Quad() {
		const std::vector<float> vertices = {
			-1.0f, -1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &VAO);
		// after binding a VAO, all subsequent calls to:
		// glEnable/DisableVertexAttribArray, glVertexAttribPointer
		// the Vetex buffer objects that are associated with the Vertex Attributes are also saved
		glBindVertexArray(VAO);
		// stores the id of the Vertex Buffer Object that is created
		glGenBuffers(1, &VBO);
		// assigns the VBO to a variable tracked by the library
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// store the vertex data in the variable tracked by the library
		  // use GL_STATIC_DRAW becaus we will use the data a lot and it won't change
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

	}
	void Init() {
	}
	void RenderFullScreen() {
		glBindVertexArray(VAO);
		quadShader.use();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};
#endif
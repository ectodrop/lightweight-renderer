#ifndef MESH3D_H

#define MESH3D_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <shader_c.h>
#include <camera_c.h>

#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

class Mesh3D {
  private:
    int numVertices = 0, numIndicies = 0;
    std::vector<Texture> textures;
    std::vector<float> vertexData;
  public:
      unsigned int VAO = 0, VBO = 0, EBO = 0;
    glm::mat4 transform = glm::mat4(1.0f);
    Mesh3D (std::vector<float> vertices, std::vector<int> attrSizeOrder) {
      int numVals = vertices.size();
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
      
      int vertexSize = 0;
      for (int i: attrSizeOrder) vertexSize += i;
      if (vertices.size() % vertexSize != 0) {
        std::cout << "Vertex attribute size mismatch" << std::endl;
      }
      numVertices = numVals / vertexSize;
      int indexOffset = 0;
      for (int i = 0; i < attrSizeOrder.size(); i++) {
        // tells opengl how to interpret the vertex attributes with respect to the currently assigned Array Buffer
        glVertexAttribPointer(i, attrSizeOrder[i], GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void *)(indexOffset * sizeof(float)));
        indexOffset += attrSizeOrder[i];
        glEnableVertexAttribArray(i);
      }

      // now it's safe to unbind the array buffer since when we call glVertexAttribPointer
      // it stores which VBO we were using in the currently bound VAO, after which it doesn't matter which VBO is bounded
       //glBindBuffer(GL_ARRAY_BUFFER, 0); 
      // "unbinds" the vertex array object, in case we make any more calls to
      // glVertexAttribPointer, glEnableVertexAttribArray etc. this rarely happens though
       //glBindVertexArray(0);
    }
    Mesh3D (std::vector<float> vertices, std::vector<int> attrSizeOrder, Texture texture) : Mesh3D(vertices, attrSizeOrder) {
      std::vector<Texture> textures;
      textures.push_back(texture);
      this->textures = textures;
      std::cout << this->textures.size() << std::endl;
    }
    Mesh3D (std::vector<float> vertices, std::vector<int> attrSizeOrder, std::vector<Texture> textures) : Mesh3D(vertices, attrSizeOrder) {
      this->textures = textures;
    }
    void subdivide(int divisions) {
      if (divisions == 0) return;
      std::vector<float> newVertices;
      std::vector<float> oldVertices = vertexData;
      for (int division = 0; division < divisions; division++) {
        
      }
    }
    void setElementBuffer(std::vector<unsigned int> indices) {
      numIndicies = indices.size();
      glBindBuffer(GL_ARRAY_BUFFER, VAO); 
      glGenBuffers(1, &EBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    /*
        Typical use would be
        shader.use();
        // set shader uniform variables
        mesh.draw(camera, shader);
    */
    void draw(Camera camera, Shader shader) {

      glBindVertexArray(VAO);
      shader.use();
      for (int i = 0; i < textures.size(); i++) {
        textures[i].activate(i);
        std::string name = "texture" + std::to_string(i);
        shader.setInt(name.c_str(), i);
      }
	    glm::mat4 view, clip;
      view = camera.viewMatrix();
      clip = camera.clipMatrix();
      shader.setMat4("model", GL_FALSE, glm::value_ptr(transform));
      shader.setMat4("view", GL_FALSE, glm::value_ptr(view));
      shader.setMat4("clip", GL_FALSE, glm::value_ptr(clip));
      if (EBO != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, numIndicies, GL_UNSIGNED_INT, 0);
      }
      else {
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
      }
      glBindVertexArray(0);
    }
    void setTextures(std::vector<Texture> textures) {
      this->textures = textures;
    }
};

#endif
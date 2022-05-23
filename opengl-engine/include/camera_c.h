#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include <cmath>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#define INITIAL_WIDTH 800
#define INITIAL_HEIGHT 600
class Camera {
  private:
    void updateCameraVectors() {
      this->right = glm::normalize(glm::cross(front, up));
    }
  public:
    float fov;
    float width, height;
    glm::vec3 pos, front, up, right;
    float pitch = 0.0f, yaw = 270.0f;
    Camera (glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float width, float height, float fov) {
      this->pos = cameraPos;
      this->front = cameraFront;
      this->up = cameraUp;
      this->width = width;
      this->height = height;
      this->fov = fov;
      updateCameraVectors();
    }
    glm::mat4 viewMatrix() {
      return glm::lookAt(pos, front + pos, up);
    }
    glm::mat4 clipMatrix() {
      return glm::perspective(glm::radians(fov), width/height, 0.1f, 100.0f);
    }
    void translate(glm::vec3 offset) {
      // translate the offset in the camera view coordinate space
      // std::cout << to_string(offset) << std::endl;
      // std::cout << to_string(pos) << std::endl;
      pos += offset;
    }
    /**
     * @brief rotate the vector representing the front of the camera
     * 
     * @param p pitch the change in rotation around the x axis in degrees
     * @param y yaw the change in rotation around the y axis in degrees
     */
    void rotate(float p, float y) {
      pitch += p;
      yaw += y;
      if (pitch > 89.0f) {
        pitch = 89.0f;
      }
      if (pitch < -89.0f) {
        pitch = -89.0f;
      }
      // std::cout << pitch << " " << yaw << std::endl;
      front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      front.y = sin(glm::radians(pitch));
      front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      front = glm::normalize(front);
      // std::cout << to_string(front) << std::endl;
      updateCameraVectors();
    }
};

#endif
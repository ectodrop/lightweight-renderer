#pragma once
#include "pch.h"
#include <accel/ray.h>
#include "constants.h"

class Camera {
private:
    float _camera_moved;
    void updateCameraVectors() {
      this->right = glm::normalize(glm::cross(front, up));
    }
    glm::mat3 AxisRotateMatrix(glm::vec3 axis, float angle) {
        axis = glm::normalize(axis);
        float c = cos(angle);
        float s = sin(angle);
        float oc = 1 - c;
        return glm::mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
            oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
            oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
    }
public:
    float fov;
    float width, height;
    glm::vec3 pos, front, up, right, starting_dir, center = glm::vec3(0);
    glm::vec2 center_offset = glm::vec2(0);
    float pitch = 0.0f, yaw = 0.0f, p_offset = 0.0f, y_offset = 0.0f;
    Camera (glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float width, float height, float fov) {
      this->pos = cameraPos;
      this->front = cameraFront;
      this->starting_dir = cameraFront;
      this->up = cameraUp;
      this->width = width;
      this->height = height;
      this->fov = fov;
      updateCameraVectors();
    }
    
    glm::vec3 GetPosition() {
        return RotationMatrix() * pos + GetCenter();
    }
    
    glm::vec3 GetCenter() {
        return center + center_offset.x * right + center_offset.y * glm::cross(right, front);
    }

    glm::mat4 ViewMatrix() {
      return glm::lookAt(GetPosition(), GetCenter(), up);
    }

    glm::mat4 ClipMatrix() {
      return glm::perspective(glm::radians(fov), width/height, 0.1f, 1000.0f);
    }
    
    glm::mat3 RotationMatrix() {
        glm::mat3 rotX = AxisRotateMatrix(glm::vec3(0, 1, 0), glm::radians(yaw + y_offset));
        glm::mat3 rotY = AxisRotateMatrix(glm::vec3(1, 0, 0), -glm::radians(pitch + p_offset));
        return rotX * rotY;
    }

    void SetMovedFlag(bool moved) {
        _camera_moved = moved;
    }

    bool MovedThisFrame() {
        return _camera_moved;
    }

    void Translate(glm::vec3 offset) {
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
    void Rotate(float p, float y) {
        p_offset = p;
        y_offset = y;
        if (pitch + p_offset > 89.0f) {
            p_offset = 89.0f - pitch;
        }
        if (pitch + p_offset < -89.0f) {
            p_offset = -89.0f - pitch;
        }
        // std::cout << pitch << " " << yaw << std::endl;
        front = glm::normalize(RotationMatrix() * starting_dir);
        // std::cout << to_string(front) << std::endl;
        updateCameraVectors();
    }


    glm::mat4 ClipToWorldSpace() {
        return glm::inverse(ClipMatrix() * ViewMatrix());
    }

    /// <summary>
    /// returns the ray rotated and offset by the transform of the camera
    /// </summary>
    /// <param name="normalized_screenspace_ray"> 2d point on the screen with values in [-1,1]</param>
    /// <returns></returns>
    Ray GetWorldSpaceRay(glm::vec2 normalized_screenspace_ray) {
        Ray ray;
        ray.dir = glm::normalize(glm::vec3(ClipToWorldSpace() * glm::vec4(normalized_screenspace_ray, 1, 1)));
        std::cout << glm::to_string(ray.dir) << std::endl;
        ray.origin = GetPosition();
        return ray;
    }

    void HandleMouseScroll(float scroll_dir) {
        float dist = glm::distance(GetCenter(), GetPosition());
        // the closer to the center the slow the scroll
        Translate(glm::normalize(starting_dir) * scroll_dir * (dist*0.1f));
        SetMovedFlag(true);
    }

    void RotateByDrag(glm::vec2 drag_delta) {
        if (drag_delta.x == 0 && drag_delta.y == 0) {
            pitch += p_offset;
            yaw += y_offset;
            p_offset = 0;
            y_offset = 0;
            if (pitch > 89.0f) {
                pitch = 89.0f;
            }
            if (pitch < -89.0f) {
                pitch = -89.0f;
            }
        }
        else {
            float sensitivity = 0.1f;
            Rotate(drag_delta.y * -1.0f * sensitivity, drag_delta.x * sensitivity);
            SetMovedFlag(true);
        }
    }

    void OffsetCenterByDrag(glm::vec2 drag_delta) {
        if (drag_delta.x == 0 && drag_delta.y == 0) {
            center = GetCenter();
            center_offset = glm::vec2(0);
        }
        else {
            float sensitivity = 0.005f;
            drag_delta.y *= -1;
            center_offset = drag_delta * sensitivity;
            SetMovedFlag(true);
        }
    }

    void HandleKeyInput(int key, int scancode, int action, int mods) {
        float movementSpeed = 0.5;
        bool key_down = action == GLFW_PRESS || action == GLFW_REPEAT;
        if (key == GLFW_KEY_LEFT_CONTROL && key_down) {
            movementSpeed *= 5.0f;
        }
        if (key == GLFW_KEY_1 && key_down) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (key == GLFW_KEY_2 && key_down) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (key == GLFW_KEY_W && key_down) {
            Translate(glm::normalize(front * glm::vec3(1, 0, 1)) * movementSpeed);
        }
        if (key == GLFW_KEY_S && key_down) {
            Translate(glm::normalize(front * glm::vec3(1, 0, 1)) * -1.0f * movementSpeed);
        }
        if (key == GLFW_KEY_D && key_down) {
            Translate(right * movementSpeed);
        }
        if (key == GLFW_KEY_A && key_down) {
            Translate(right * -1.0f * movementSpeed);
        }
        if (key == GLFW_KEY_SPACE && key_down) {
            Translate(up * movementSpeed);
        }
        if (key == GLFW_KEY_LEFT_SHIFT && key_down) {
            Translate(up * -1.0f * movementSpeed);
        }
    }
};

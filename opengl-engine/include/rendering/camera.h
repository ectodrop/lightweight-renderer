#pragma once
#include "pch.h"
#include <accel/ray.h>
#include "constants.h"

class Camera {
public:
    float fov;
    float _Width, _Height;
    glm::vec3 _pos, _front, _up, _right, _starting_dir, _center = glm::vec3(0);
    glm::vec2 _center_offset = glm::vec2(0);
    float _pitch = 0.0f, _yaw = 0.0f, _p_offset = 0.0f, _y_offset = 0.0f;
    Camera (glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float width, float height, float fov) {
      this->_pos = cameraPos;
      this->_front = cameraFront;
      this->_starting_dir = cameraFront;
      this->_up = cameraUp;
      this->_Width = width;
      this->_Height = height;
      this->fov = fov;
      UpdateCameraVectors();
    }
    
    glm::vec3 GetPosition() {
        return RotationMatrix() * _pos + GetCenter();
    }
    
    glm::vec3 GetCenter() {
        return _center + _center_offset.x * _right + _center_offset.y * glm::cross(_right, _front);
    }

    glm::mat4 ViewMatrix() {
      return glm::lookAt(GetPosition(), GetCenter(), _up);
    }

    glm::mat4 ClipMatrix() {
      return glm::perspective(glm::radians(fov), _Width/_Height, 0.1f, 1000.0f);
    }
    
    glm::mat3 RotationMatrix() {
        glm::mat3 rotX = AxisRotateMatrix(glm::vec3(0, 1, 0), glm::radians(_yaw + _y_offset));
        glm::mat3 rotY = AxisRotateMatrix(glm::vec3(1, 0, 0), -glm::radians(_pitch + _p_offset));
        return rotX * rotY;
    }

    void InitSettings(glm::vec3 pos, glm::vec3 center, float pitch, float yaw) {
        _pos = pos;
        _center = center;
        _pitch = pitch;
        _yaw = yaw;
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
      _pos += offset;
    }
    /**
     * @brief rotate the vector representing the front of the camera
     * 
     * @param p pitch the change in rotation around the x axis in degrees
     * @param y yaw the change in rotation around the y axis in degrees
     */
    void Rotate(float p, float y) {
        _p_offset = p;
        _y_offset = y;
        if (_pitch + _p_offset > 89.0f) {
            _p_offset = 89.0f - _pitch;
        }
        if (_pitch + _p_offset < -89.0f) {
            _p_offset = -89.0f - _pitch;
        }
        // std::cout << pitch << " " << yaw << std::endl;
        _front = glm::normalize(RotationMatrix() * _starting_dir);
        // std::cout << to_string(front) << std::endl;
        UpdateCameraVectors();
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
        Translate(glm::normalize(_starting_dir) * scroll_dir * (dist*0.1f));
        SetMovedFlag(true);
    }

    void RotateByDrag(glm::vec2 drag_delta) {
        if (drag_delta.x == 0 && drag_delta.y == 0) {
            _pitch += _p_offset;
            _yaw += _y_offset;
            _p_offset = 0;
            _y_offset = 0;
            if (_pitch > 89.0f) {
                _pitch = 89.0f;
            }
            if (_pitch < -89.0f) {
                _pitch = -89.0f;
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
            _center = GetCenter();
            _center_offset = glm::vec2(0);
        }
        else {
            float sensitivity = 0.005f;
            drag_delta.y *= -1;
            _center_offset = drag_delta * sensitivity;
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
            Translate(glm::normalize(_front * glm::vec3(1, 0, 1)) * movementSpeed);
        }
        if (key == GLFW_KEY_S && key_down) {
            Translate(glm::normalize(_front * glm::vec3(1, 0, 1)) * -1.0f * movementSpeed);
        }
        if (key == GLFW_KEY_D && key_down) {
            Translate(_right * movementSpeed);
        }
        if (key == GLFW_KEY_A && key_down) {
            Translate(_right * -1.0f * movementSpeed);
        }
        if (key == GLFW_KEY_SPACE && key_down) {
            Translate(_up * movementSpeed);
        }
        if (key == GLFW_KEY_LEFT_SHIFT && key_down) {
            Translate(_up * -1.0f * movementSpeed);
        }
    }
private:
    float _camera_moved;
    void UpdateCameraVectors() {
        this->_right = glm::normalize(glm::cross(_front, _up));
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
};

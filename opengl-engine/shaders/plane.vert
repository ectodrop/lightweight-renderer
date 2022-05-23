#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 position;
out vec3 normal;
uniform float offset;

uniform float near;
uniform float far;

uniform mat4 model;
uniform mat4 view;
uniform mat4 clip; 

void main() {
  // float x = aPos.x * cos(offset) - aPos.y * sin(offset);
  // float y = aPos.y * cos(offset) + aPos.x * sin(offset);
  normal = aNormal;
  position = vec4(aPos * far, 1.0f);
  gl_Position = clip * view * model * position;
}
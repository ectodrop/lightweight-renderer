#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 worldPos;
out vec3 texcoord;
out vec3 normal;
uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 clip; 


void main() {
  // float x = aPos.x * cos(offset) - aPos.y * sin(offset);
  // float y = aPos.y * cos(offset) + aPos.x * sin(offset); 

  texcoord = aPos;
  worldPos = vec3(model * vec4(aPos, 1.0f));
  // gl_Position = clip * view * model * vec4(aPos, 1.0f); 
  gl_Position = clip * view * model * vec4(aPos, 1.0f);
  normal = mat3(transpose(inverse(model))) *aNormal;
}
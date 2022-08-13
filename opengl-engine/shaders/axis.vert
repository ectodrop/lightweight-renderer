#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 worldPos;
uniform mat4 view;
uniform mat4 clip; 

void main() {
  // float x = aPos.x * cos(offset) - aPos.y * sin(offset);
  // float y = aPos.y * cos(offset) + aPos.x * sin(offset); 
  
  vec4 pos = clip * view * vec4(aPos, 1.0f);
  gl_Position = pos;
  worldPos = aPos;
  
}
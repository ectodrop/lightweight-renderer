#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;

out vec3 color;
out vec4 position;
out vec2 texcoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 clip; 
uniform float offset;

void main() {
  // float x = aPos.x * cos(offset) - aPos.y * sin(offset);
  // float y = aPos.y * cos(offset) + aPos.x * sin(offset);
  position = clip * view * model * vec4(aPos, 1.0f);
  gl_Position = position;
  
  color = aCol;
  texcoord = aTexCoord;
}
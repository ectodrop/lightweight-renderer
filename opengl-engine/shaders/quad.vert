#version 460 core
layout (location = 0) in vec3 aPos;

out vec4 position;
out vec2 texcoord;

void main() {
  gl_Position = vec4(aPos, 1.0f);
  texcoord = aPos.xy/2.0f + 0.5f;
}
#version 460 core
in vec3 worldPos;

layout(location = 0) out vec4 color;
uniform float offset;
uniform vec3 cameraPos;
const float radius = 5.0f;



void main() {
  // vec4 objectColor = texture(texture0, texcoord);

  color = vec4(normalize(abs(worldPos)), 1);
}

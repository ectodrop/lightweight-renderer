#version 330 core
in vec4 position;
in vec3 color;
in vec2 texcoord;

out vec4 FragColor;

uniform float offset;
uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
  // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  // FragColor = vec4(color, 0.2f);
  FragColor = mix(texture(texture0, texcoord), texture(texture1, texcoord), 0.2);
}

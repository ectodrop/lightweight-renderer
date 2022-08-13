#version 460 core
in vec3 worldPos;
in vec3 texcoord;
in vec3 normal;

layout(location = 0) out vec4 color;
uniform float offset;
uniform vec3 cameraPos;
const float radius = 5.0f;

uniform vec3 diffuseBase;
uniform sampler2D diffuseMap;

vec3 lightPos = vec3(5,5,5);

void main() {
  // vec4 objectColor = texture(texture0, texcoord);
  // vec4 objectColor = vec4(diffuseBase,1);

  float diffuse = dot(normal, normalize(lightPos - worldPos));
  //color = texture(diffuseMap, texcoord.xy) * objectColor;
  
  color = vec4(texcoord, 1);
}

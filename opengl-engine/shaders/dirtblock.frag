#version 330 core
in vec3 worldPos;
in vec3 texcoord;
in vec3 normal;

out vec4 FragColor;

uniform float offset;
uniform samplerCube texture0;
uniform vec3 lightPos;
uniform vec3 cameraPos;
const float radius = 5.0f;
const vec3 lightColor = vec3(1.0f);
const vec3 ambient = vec3(0.1f);
const float lightIntensity = 0.5f;
const float specularStrength = 0.5f;
void main() {
  // vec4 objectColor = texture(texture0, texcoord);
  vec4 objectColor = vec4(1.0, 0.5, 0.3, 1.0);


  vec3 lightDir = normalize(lightPos - worldPos);
  float diff = max(dot(normal, lightDir), 0.0f);
  vec3 diffuse = (radius/length(lightPos - worldPos)) * diff * lightColor; // make the diffuse weaker based on distance from light
  vec3 cameraDir = normalize(cameraPos - worldPos);
  vec3 reflectDir = reflect(-lightDir, normal);

  float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 256);
  vec3 specular = specularStrength * spec * lightColor;
  FragColor = vec4(ambient + diffuse + specular, 1.0f) * objectColor;
  // FragColor = vec4(normal, 1.0f);
}

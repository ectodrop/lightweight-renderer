#version 330 core
in vec3 worldPos;
in vec3 texcoord;
in vec3 normal;

out vec4 FragColor;

uniform sampler2D texture0;
uniform vec3 lightPos;
uniform vec3 cameraPos;
const float radius = 1.0f;
const vec3 lightColor = vec3(1.0f);
const vec3 ambient = vec3(0.1f);
const float specularStrength = 1.0f;
const float gamma = 2.2f;
void main() {
  // vec4 objectColor = texture(texture0, texcoord);
  vec4 objectColor = pow(vec4(1.0, 0.5, 0.3, 1.0), vec4(gamma));

  vec3 lightDir = normalize(lightPos - worldPos);
  float diff = max(dot(normal, lightDir), 0.0f);
  float dist = length(lightPos - worldPos);
  float falloff = (radius/(dist*dist));
  vec3 diffuse = falloff * diff * lightColor; // make the diffuse weaker based on distance from light
  // vec3 cameraDir = normalize(cameraPos - worldPos);
  // vec3 reflectDir = reflect(-lightDir, normal);
  // float spec = pow(max(dot(cameraDir, reflectDir), 0.0f), 32);
  // vec3 specular = specularStrength * spec * lightColor;
  vec3 cameraDir = normalize(cameraPos - worldPos);
  vec3 halfway = normalize(cameraDir + lightDir);
  float spec = pow(max(dot(normalize(normal), halfway), 0.0f), 512);
  vec3 specular = falloff * specularStrength * spec * lightColor;
  FragColor = pow(vec4(ambient + diffuse + specular, 1.0f) * objectColor, vec4(1/gamma));
  // FragColor = vec4(abs(normal), 1.0f);
}

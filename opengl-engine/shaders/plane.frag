#version 460 core
in vec4 position;
in vec3 normal;

out vec4 FragColor;

uniform sampler2D texture0;
uniform vec3 lightPos;
uniform vec3 cameraPos;
const float radius = 1.0f;
const vec3 lightColor = vec3(1.0f);
const vec3 ambient = vec3(0.1f);
const float lightIntensity = 0.5f;
const float specularStrength = 0.5f;
const float gamma = 2.2f;
void main() {
  vec3 worldPos = vec3(position);
  vec4 objectColor = pow(texture(texture0, position.xz), vec4(gamma));
  // flat shading
  vec3 lightDir = normalize(lightPos - worldPos);
  float diff = max(dot(normal, lightDir), 0.0f);
  float dist = length(lightPos - worldPos);
  vec3 diffuse = (radius/(dist * dist)) * diff * lightColor; // make the diffuse weaker based on distance from light
  // phong specular lighting
  // vec3 cameraDir = normalize(cameraPos - worldPos);
  // vec3 reflectDir = reflect(-lightDir, normal);
  // float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 1);
  // vec3 specular = specularStrength * spec * lightColor;
  
  // blinn-phong specular lighting
  vec3 cameraDir = normalize(cameraPos - worldPos);
  vec3 halfway = normalize(cameraDir + lightDir);
  float spec = pow(max(dot(normal, halfway), 0.0), 512);
  
  vec3 specular = diff * specularStrength * spec * lightColor;
  FragColor = pow(vec4(ambient + diffuse + specular, 1.0f) * objectColor, vec4(1/gamma));
}

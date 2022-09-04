#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec3 aTex;

out vec3 worldPos;
out vec3 texcoord;
out vec3 normal;
out mat3 TBN;
uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 clip; 
uniform float offset;

mat3 getTBN() {
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	return mat3(T,B,N);
}
void main() {
  // float x = aPos.x * cos(offset) - aPos.y * sin(offset);
  // float y = aPos.y * cos(offset) + aPos.x * sin(offset); 

  texcoord = aTex;
  gl_Position = vec4(aPos, 1);//vec3(model * vec4(aPos, 1.0f));
  gl_Position = clip * view * model * vec4(aPos, 1.0f);
  // gl_Position = clip * view * model * vec4(aPos, 1.0f);
  normal = normalize(mat3(transpose(inverse(model))) * aNormal);
  TBN = getTBN();
}
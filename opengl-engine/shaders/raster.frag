#version 460 core
in vec3 worldPos;
in vec3 texcoord;
in vec3 normal;
in mat3 TBN;

out vec4 color;
uniform float offset;
uniform vec3 cameraPos;
const float radius = 5.0f;

uniform vec3 diffuseBase;
uniform bool hasDiffuseMap;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

vec3 lightDir = vec3(-1,-1,-1);

void main() {
  // vec4 objectColor = texture(texture0, texcoord);
  vec4 objectColor = vec4(diffuseBase,1);
  //vec4 texColor = texture(diffuseMap, texcoord.xy);
  //vec3 texNormal = texture(normalMap, texcoord.xy).xyz;
  //texNormal = texNormal * 2.0 - 1.0;
  //texNormal = normalize(TBN * texNormal);
  //vec4 ambient = vec4(0.2);
  float lambert = dot(normal, normalize(-lightDir));
  color = objectColor * abs(lambert);
  
  // color = vec4(normal, 1);
}

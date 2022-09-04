#pragma once
#include "pch.h"

struct Material {
	glm::vec3 Diffuse = glm::vec3(0.8);
	glm::vec3 Specular = glm::vec3(1.0);
	glm::vec3 Normal = glm::vec3(0.0);
	glm::vec3 Emissive = glm::vec3(0.0);
	int Diffuse_Index = -1;
	int Specular_Index = -1;
	int Normal_Index = -1;
	int Emissive_Index = -1;
};
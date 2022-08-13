#pragma once
#include "pch.h"

struct Material {
	glm::vec3 Diffuse = glm::vec3(0.8);
	glm::vec3 Specular = glm::vec3(1.0);
	int Diffuse_Index = -1;
	int Specular_Index = -1;
	int Normal_Index = -1;
};
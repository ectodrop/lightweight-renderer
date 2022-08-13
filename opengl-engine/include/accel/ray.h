#pragma once
#include "pch.h"

struct Ray {
	Ray() : max_t(std::numeric_limits<float>::infinity()), origin(glm::vec3(0)), dir(glm::vec3(0)) {}
	glm::vec3 origin, dir;
	float max_t;
};

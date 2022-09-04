#pragma once
#include "pch.h"
#include "bounds.h"
#include "ray.h"

struct Primative {
	Primative(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) : vertex0(v0), vertex1(v1), vertex2(v2) {
		centroid = (v0 + v1 + v2) / 3.0f;
		bcentroid = GetBounds().centroid();
	}
	Bound GetBounds() {
		return Bound(glm::min(vertex0, vertex1, vertex2), glm::max(vertex0, vertex1, vertex2));
	}
	bool IntersectTri(Ray ray, float& hit) {
		glm::vec3 p0 = vertex0;
		glm::vec3 p1 = vertex1;
		glm::vec3 p2 = vertex2;

		glm::vec3 e0 = p1 - p0;
		glm::vec3 e1 = p0 - p2;
		glm::vec3 triangleNormal = glm::cross(e1, e0);

		glm::vec3 e2 = (1.0f / glm::dot(triangleNormal, ray.dir)) * (p0 - ray.origin);
		glm::vec3 i = glm::cross(ray.dir, e2);
		glm::vec3 barycentricCoord;
		barycentricCoord.y = glm::dot(i, e1);
		barycentricCoord.z = glm::dot(i, e0);
		barycentricCoord.x = 1.0f - (barycentricCoord.z + barycentricCoord.y);
		hit = glm::dot(triangleNormal, e2);

		return  /*(hit < ray.tmax) && */ (hit > 0.000001) && glm::all(glm::greaterThanEqual(barycentricCoord, glm::vec3(0.0)));
	}
	glm::vec3 vertex0, vertex1, vertex2, centroid, bcentroid;
};

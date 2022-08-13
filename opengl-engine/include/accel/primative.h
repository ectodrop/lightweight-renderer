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
	bool IntersectTri(Ray ray, float& t) {
		glm::vec3 dir = ray.dir;
		glm::vec3 orig = ray.origin;
		auto v0 = vertex0, v1 = vertex1, v2 = vertex2;
		// compute plane's normal
		glm::vec3 v0v1 = v1 - v0;
		glm::vec3 v0v2 = v2 - v0;
		// no need to normalize
		glm::vec3 N = glm::cross(v0v1, v0v2);  //N 
		float area2 = N.length();

		// Step 1: finding P

		// check if ray and plane are parallel ?
		float NdotRayDirection = glm::dot(N, dir);
		if (fabs(NdotRayDirection) <= std::numeric_limits<float>::epsilon())  //almost 0 
			return false;  //they are parallel so they don't intersect ! 

		// compute d parameter using equation 2
		float d = glm::dot(-N, v0);

		// compute t (equation 3)
		t = -(glm::dot(N, orig) + d) / NdotRayDirection;

		// check if the triangle is in behind the ray
		if (t < 0) return false;  //the triangle is behind 

		// compute the intersection point using equation 1
		glm::vec3 P = orig + t * dir;

		// Step 2: inside-outside test
		glm::vec3 C;  //vector perpendicular to triangle's plane 

		// edge 0
		glm::vec3 edge0 = v1 - v0;
		glm::vec3 vp0 = P - v0;
		C = glm::cross(edge0, vp0);
		if (glm::dot(N, C) < 0) return false;  //P is on the right side 

		// edge 1
		glm::vec3 edge1 = v2 - v1;
		glm::vec3 vp1 = P - v1;
		C = glm::cross(edge1, vp1);
		if (glm::dot(N, C) < 0)  return false;  //P is on the right side 

		// edge 2
		glm::vec3 edge2 = v0 - v2;
		glm::vec3 vp2 = P - v2;
		C = glm::cross(edge2, vp2);
		if (glm::dot(N, C) < 0) return false;  //P is on the right side; 

		return true;  //this ray hits the triangle
	}
	glm::vec3 vertex0, vertex1, vertex2, centroid, bcentroid;
};

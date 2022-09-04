#pragma once
#include "ray.h"

struct Bound {
	Bound(glm::vec3 aa, glm::vec3 bb) {
		bmin = aa;
		bmax = bb;
	}
	Bound() {
		bmin = glm::vec3(std::numeric_limits<double>::infinity());
		bmax = glm::vec3(-std::numeric_limits<double>::infinity());
	}

	void Union(Bound other) {
		bmin = glm::min(bmin, other.bmin);
		bmax = glm::max(bmax, other.bmax);
	}

	int LongestAxis() {
		auto extent = bmax - bmin;
		int axis = 0;
		if (extent.y > extent.x) axis = 1;
		if (extent.z > extent.y) axis = 2;
		return axis;
	}

	float SurfaceArea() {
		return 2 * (extent().x * extent().y + extent().x * extent().z + extent().y * extent().z);
	}

	bool AABBIntersect(Ray r, float& t) {
		glm::vec3 invdir = glm::vec3(1.0) / r.dir;

		glm::vec3 f = (bmax - r.origin) * invdir;
		glm::vec3 n = (bmin - r.origin) * invdir;

		glm::vec3 tmax = max(f, n);
		glm::vec3 tmin = min(f, n);

		float t1 = glm::min(tmax.x, glm::min(tmax.y, tmax.z));
		float t0 = glm::max(tmin.x, glm::max(tmin.y, tmin.z));

		t = (t1 >= t0) ? (t0 > 0.f ? t0 : t1) : -1.0;
		return t >= 0.0f;
	}

	glm::vec3 extent() {
		return bmax - bmin;
	}

	glm::vec3 centroid() {
		return (bmin + bmax) / 2.0f;
	}

	glm::vec3 bmin, bmax;
};

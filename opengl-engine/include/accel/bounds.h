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
		float tmin, tmax, tymin, tymax, tzmin, tzmax;
		glm::vec3 bounds[2] = { bmin, bmax };
		glm::vec3 invdir = 1.0f / r.dir;
		glm::bvec3 sign(invdir.x < 0, invdir.y < 0, invdir.z < 0);
		tmin = (bounds[sign[0]].x - r.origin.x) * invdir.x;
		tmax = (bounds[1 - sign[0]].x - r.origin.x) * invdir.x;
		tymin = (bounds[sign[1]].y - r.origin.y) * invdir.y;
		tymax = (bounds[1 - sign[1]].y - r.origin.y) * invdir.y;

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;

		tzmin = (bounds[sign[2]].z - r.origin.z) * invdir.z;
		tzmax = (bounds[1 - sign[2]].z - r.origin.z) * invdir.z;

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;

		t = tmin;

		if (t < 0) {
			t = tmax;
			if (t < 0) return false;
		}

		return true;
	}

	glm::vec3 extent() {
		return bmax - bmin;
	}

	glm::vec3 centroid() {
		return (bmin + bmax) / 2.0f;
	}

	glm::vec3 bmin, bmax;
};

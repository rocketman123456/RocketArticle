#include "Scene/Component/Model.h"

using namespace Rocket;

#include <glm/glm.hpp>

BoundingBox BoundingBox::GetAABB(const Matrix4f& m)
{
	Vector3f min = m.block<3, 1>(0, 3); // glm::vec3(m[3]);
	Vector3f max = min;
	Vector3f v0, v1;

	Vector3f right = m.block<3, 1>(0, 0); //glm::vec3(m[0]);
	v0 = right * this->minPos[0];
	v1 = right * this->maxPos[0];
	min += GetMin(v0, v1); //glm::min(v0, v1);
	max += GetMax(v0, v1); // glm::max(v0, v1);

	Vector3f up = m.block<3, 1>(0, 1); //glm::vec3(m[1]);
	v0 = up * this->minPos[1];
	v1 = up * this->maxPos[1];
	min += GetMin(v0, v1); //glm::min(v0, v1);
	max += GetMax(v0, v1); //glm::max(v0, v1);

	Vector3f back = m.block<3, 1>(0, 2); //glm::vec3(m[2]);
	v0 = back * this->minPos[2];
	v1 = back * this->maxPos[2];
	min += GetMin(v0, v1); //glm::min(v0, v1);
	max += GetMax(v0, v1); //glm::max(v0, v1);

	return BoundingBox(min, max);
}

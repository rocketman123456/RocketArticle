#pragma once
#include "Scene/SceneComponent.h"

#include <glm/glm.hpp>

namespace Rocket
{
	struct ModelVertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv0;
		glm::vec2 uv1;
		glm::vec4 joint0;
		glm::vec4 weight0;
	};

    Interface Mesh : implements SceneComponent
    {
    public:
        virtual ~Mesh() = default;
    };
}
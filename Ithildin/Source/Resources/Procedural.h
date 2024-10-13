#pragma once
#include "Math/Math.h"
#include <utility>

namespace Resources
{
    class Procedural
    {
    public:
        Procedural() = default;
        virtual ~Procedural() = default;
        virtual std::pair<glm::vec3, glm::vec3> GetBoundingBox() const = 0;
    };
}
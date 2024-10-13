#pragma once
#include "Procedural.h"
#include "Math/Math.h"

namespace Resources
{
    class Sphere : public Procedural
    {
    public:
        Sphere(const glm::vec3& center, const float radius) : m_Center(center), m_Radius(radius)
        {

        }

        std::pair<glm::vec3, glm::vec3> GetBoundingBox() const override
        {
            return std::make_pair(m_Center - m_Radius, m_Center + m_Radius);
        }

    public:
        const glm::vec3 m_Center;
        const float m_Radius;
    };
}
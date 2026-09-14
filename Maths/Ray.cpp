#include "Ray.hpp"

float Ray::AABBIntersection(const glm::vec3 &min, const glm::vec3 &max)
{
    float tmin = (min.x - origin.x) / direction.x;
    float tmax = (max.x - origin.x) / direction.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (min.y - origin.y) / direction.y;
    float tymax = (max.y - origin.y) / direction.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if (tmin > tymax || tymin > tmax)
        return -1;

    tmin = glm::max(tmin, tymin);
    tmax = glm::min(tmax, tymax);

    float tzmin = (min.z - origin.z) / direction.z;
    float tzmax = (max.z - origin.z) / direction.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if (tmin > tzmax || tzmin > tmax)
        return -1;

    tmin = glm::max(tmin, tzmin);
    tmax = glm::min(tmax, tzmax);

    float t = glm::min(tmin, tmax);

    return t;
}
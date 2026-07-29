#include "Random.hpp"

glm::vec4 RandomUnitVec4()
{
    return {float(rand()) / float(RAND_MAX), float(rand()) / float(RAND_MAX), float(rand()) / float(RAND_MAX), float(rand()) / float(RAND_MAX)};
}

glm::vec3 RandomUnitVec3()
{
    return RandomUnitVec4();
}

glm::vec2 RandomUnitVec2()
{
    return RandomUnitVec4();
}

float RandomUnitFloat()
{
    return RandomUnitVec4().x;
}
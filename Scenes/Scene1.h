#pragma once

#include "Scene.h"
#include <glm/glm.hpp>
#include <array>

class Scene1 : public Scene
{
public:
    void init() override;

private:

    struct MassPoint
    {
        glm::vec3 position;
        glm::vec3 velocity;
        float mass;
    };

    struct SystemState
    {
        std::array<MassPoint, 2> points;
    };

    // System parameters
    float stiffness = 40.0f;
    float restLength = 1.0f;
    float dt = 0.1f;

    // Initial system state
    SystemState initialState;

    // Physics and integration functions
    std::array<glm::vec3, 2> computeAccelerations(const SystemState& state) const;
    SystemState integrateEuler(const SystemState& state);
    SystemState integrateMidpoint(const SystemState& state);

    // Output
    void printState(const SystemState& state);
};
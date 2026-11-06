#include "Scene1.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

void Scene1::init()
{
    // Initialize mass point 0
    initialState.points[0] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            10.0f
    };

    // Initialize mass point 1
    initialState.points[1] = {
            glm::vec3(0.0f, 2.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            10.0f
    };

    // Print initial values
    std::cout << "Initial Values:" << std::endl;
    printState(initialState);

    // Euler integration
    SystemState eulerState = integrateEuler(initialState);

    std::cout << "\nAfter 0.1s using Euler Method:" << std::endl;
    printState(eulerState);

    // Midpoint integration
    SystemState midpointState = integrateMidpoint(initialState);

    std::cout << "\nAfter 0.1s using Midpoint Method:" << std::endl;
    printState(midpointState);
}

std::array<glm::vec3, 2>
Scene1::computeAccelerations(const SystemState& state) const
{
    glm::vec3 direction =
            state.points[1].position - state.points[0].position;

    float length = glm::length(direction);

    glm::vec3 force0(0.0f);
    glm::vec3 force1(0.0f);

    if (length > 0.0f)
    {
        force0 = stiffness * (length - restLength)
                 * direction / length;

        force1 = -force0;
    }

    std::array<glm::vec3, 2> accelerations{};

    accelerations[0] = force0 / state.points[0].mass;
    accelerations[1] = force1 / state.points[1].mass;

    return accelerations;
}

Scene1::SystemState
Scene1::integrateEuler(const SystemState& state)
{
    SystemState result = state;

    auto accelerations = computeAccelerations(state);

    for (int i = 0; i < 2; i++)
    {
        result.points[i].position =
                state.points[i].position + dt * state.points[i].velocity;

        result.points[i].velocity =
                state.points[i].velocity + dt * accelerations[i];
    }

    return result;
}

Scene1::SystemState
Scene1::integrateMidpoint(const SystemState& state)
{
    SystemState midpoint = state;

    // Calculate accelerations at the initial state
    auto accelerations = computeAccelerations(state);

    // Calculate midpoint positions and velocities
    for (int i = 0; i < 2; i++)
    {
        midpoint.points[i].position =
                state.points[i].position
                + 0.5f * dt * state.points[i].velocity;

        midpoint.points[i].velocity =
                state.points[i].velocity
                + 0.5f * dt * accelerations[i];
    }

    // Calculate accelerations at the midpoint
    auto midpointAccelerations = computeAccelerations(midpoint);

    // Calculate final state using midpoint values
    SystemState result = state;

    for (int i = 0; i < 2; i++)
    {
        result.points[i].position =
                state.points[i].position
                + dt * midpoint.points[i].velocity;

        result.points[i].velocity =
                state.points[i].velocity
                + dt * midpointAccelerations[i];
    }

    return result;
}

void Scene1::printState(const SystemState& state)
{
    for (int i = 0; i < 2; i++)
    {
        std::cout << "Masspoint " << i << std::endl;

        std::cout << "position: "
                  << glm::to_string(state.points[i].position)
                  << std::endl;

        std::cout << "velocity: "
                  << glm::to_string(state.points[i].velocity)
                  << std::endl;
    }
}
#pragma once

#include "Scene.h"
#include <glm/glm.hpp>
#include <vector>

class Scene4 : public Scene
{
public:
    void init() override;
    void simulateStep() override;
    void onDraw(Renderer& renderer) override;
    void onGUI() override;

private:
    struct MassPoint
    {
        glm::vec3 position;
        glm::vec3 velocity;
        float mass = 1.0f;
    };

    struct Spring
    {
        int pointA;
        int pointB;
        float restLength;
        float stiffness;
    };

    std::vector<MassPoint> points;
    std::vector<Spring> springs;

    float dt = 0.005f;
    float gravity = 9.81f;

    // 0 = Euler, 1 = Midpoint
    int method = 0;

    int selectedPoint = 0;

    void resetSimulation();

    std::vector<glm::vec3> computeAccelerations(
        const std::vector<glm::vec3>& positions) const;

    void resolveGroundCollision(
        std::vector<glm::vec3>& positions,
        std::vector<glm::vec3>& velocities) const;
};

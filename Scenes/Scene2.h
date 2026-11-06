#pragma once

#include "Scene.h"
#include <glm/glm.hpp>

class Scene2 : public Scene
{
public:
    void init() override;
    void simulateStep() override;
    //void onDraw(Renderer& renderer) override;
    void onGUI() override;

private:
    glm::vec3 x0;
    glm::vec3 x1;
    glm::vec3 v0;
    glm::vec3 v1;

    float m_0 = 10.0f;
    float m_1 = 10.0f;
    float k = 40.0f; // stiffness

    float L = 1.0f; // rest length
    float dt = 0.005f;
};
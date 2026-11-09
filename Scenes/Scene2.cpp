#include "Scene2.h"
#include <imgui.h>

void Scene2::init()
{
    x0 = glm::vec3(0.0f, 0.0f, 0.0f);
    v0 = glm::vec3(-1.0f, 0.0f, 0.0f);

    x1 = glm::vec3(0.0f, 2.0f, 0.0f);
    v1 = glm::vec3(1.0f, 0.0f, 0.0f);

    dt = 0.005f;
}

void Scene2::onDraw(Renderer& renderer)
{
    // Draw the two masses
    renderer.drawSphere(x0, 0.1f, glm::vec4(1, 0, 0, 1));
    renderer.drawSphere(x1, 0.1f, glm::vec4(0, 0, 1, 1));

    // Draw the spring connecting the masses
    renderer.drawLine(x0, x1, glm::vec3(1, 1, 1));

    renderer.drawWireCube(glm::vec3(0), glm::vec3(5), glm::vec3(1));
}

void Scene2::simulateStep()
{
    glm::vec3 d = x1 - x0;
    float l = glm::length(d);
    std::cout << "Spring length: " << l << std::endl;

    if (l > 0) {

        glm::vec3 F_0 =
                k * (l - L) * d / l;

        glm::vec3 F_1 = -F_0;

        glm::vec3 a_0 = F_0 / m_0;
        glm::vec3 a_1 = F_1 / m_1;

        // Calculate new velocities
        glm::vec3 newV_0 = v0 + dt * a_0;
        glm::vec3 newV_1 = v1 + dt * a_1;

        // Calculate new positions using the old velocities
        glm::vec3 newX_0 = x0 + dt * v0;
        glm::vec3 newX_1 = x1 + dt * v1;

        // Update the state
        v0 = newV_0;
        v1 = newV_1;
        x0 = newX_0;
        x1 = newX_1;
    }
}

void Scene2::onGUI()
{
    ImGui::DragFloat(
            "Time step",
            &dt,
            0.0005f,
            0.0001f,
            0.05f,
            "%.4f"
    );
}
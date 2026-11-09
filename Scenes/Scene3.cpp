#include "Scene3.h"
#include <imgui.h>

void Scene3::init()
{
    x0 = glm::vec3(0.0f, 0.0f, 0.0f);
    v0 = glm::vec3(-1.0f, 0.0f, 0.0f);

    x1 = glm::vec3(0.0f, 2.0f, 0.0f);
    v1 = glm::vec3(1.0f, 0.0f, 0.0f);

    dt = 0.005f;
}

void Scene3::onDraw(Renderer& renderer)
{
    // Draw the two masses
    renderer.drawSphere(x0, 0.1f, glm::vec4(1, 0, 0, 1));
    renderer.drawSphere(x1, 0.1f, glm::vec4(0, 0, 1, 1));

    // Draw the spring connecting the masses
    renderer.drawLine(x0, x1, glm::vec3(1, 1, 1));

    renderer.drawWireCube(glm::vec3(0), glm::vec3(5), glm::vec3(1));
}

void Scene3::simulateStep()
{
    // 1. Calculate forces at the beginning of the time step
    glm::vec3 d = x1 - x0;
    float l = glm::length(d);

    if (l > 0) {
        glm::vec3 force0 =
            k * (l - L) * d / l;
        glm::vec3 force1 = -force0;

        glm::vec3 a0 = force0 / m_0;
        glm::vec3 a1 = force1 / m_1;

        // 2. Estimate midpoint velocities and positions
        glm::vec3 v0_mid = v0 + 0.5f * dt * a0;
        glm::vec3 v1_mid = v1 + 0.5f * dt * a1;

        glm::vec3 x0_mid = x0 + 0.5f * dt * v0;
        glm::vec3 x1_mid = x1 + 0.5f * dt * v1;

        // 3. Calculate forces at the midpoint
        glm::vec3 d_mid = x1_mid - x0_mid;
        float length_mid = glm::length(d_mid);


        if (length_mid > 0)
        {
            glm::vec3 force0_mid =
                k * (length_mid - L) * d_mid / length_mid;
            glm::vec3 force1_mid = -force0_mid;

            glm::vec3 a0_mid = force0_mid / m_0;
            glm::vec3 a1_mid = force1_mid / m_1;

            // 4. Update positions and velocities using midpoint values
            x0 += dt * v0_mid;
            x1 += dt * v1_mid;

            v0 += dt * a0_mid;
            v1 += dt * a1_mid;
        }
    }
}

void Scene3::onGUI()
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
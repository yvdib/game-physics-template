#include "Scene4.h"
#include <imgui.h>
#include <cmath>

void Scene4::init()
{
    dt = 0.005f;
    gravity = 9.81f;
    method = 0;

    resetSimulation();
}

void Scene4::resetSimulation()
{
    points.clear();
    springs.clear();

    const int rows = 3;
    const int columns = 4;
    const float spacing = 0.7f;
    const float height = 2.5f;
    const float stiffness = 80.0f;

    // Create mass points
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            MassPoint point;

            point.position = glm::vec3(
                (col - 1.5f) * spacing,
                height,
                (row - 1.0f) * spacing
            );

            point.velocity = glm::vec3(0.0f);
            point.mass = 1.0f;

            points.push_back(point);
        }
    }

    // Create horizontal and vertical springs
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            int current = row * columns + col;

            // Connect to the neighbour on the right
            if (col < columns - 1)
            {
                int right = current + 1;

                springs.push_back({
                    current,
                    right,
                    spacing,
                    stiffness
                });
            }

            // Connect to the neighbour below
            if (row < rows - 1)
            {
                int below = current + columns;

                springs.push_back({
                    current,
                    below,
                    spacing,
                    stiffness
                });
            }
        }
    }

    selectedPoint = 0;
}

std::vector<glm::vec3> Scene4::computeAccelerations(
    const std::vector<glm::vec3>& positions) const
{
    std::vector<glm::vec3> forces(points.size());

    // Gravity
    for (int i = 0; i < points.size(); i++)
    {
        forces[i] = glm::vec3(0.0f, -gravity * points[i].mass, 0.0f);
    }

    // Spring forces
    for (const Spring& spring : springs)
    {
        int a = spring.pointA;
        int b = spring.pointB;

        glm::vec3 d = positions[b] - positions[a];
        float length = glm::length(d);

        if (length < 1e-6f)
            continue;

        glm::vec3 force =
            spring.stiffness *
            (length - spring.restLength) *
            d / length;

        forces[a] += force;
        forces[b] -= force;
    }

    // Convert forces into accelerations
    std::vector<glm::vec3> accelerations(points.size());

    for (int i = 0; i < points.size(); i++)
    {
        accelerations[i] = forces[i] / points[i].mass;
    }

    return accelerations;
}

void Scene4::resolveGroundCollision(
    std::vector<glm::vec3>& positions,
    std::vector<glm::vec3>& velocities) const
{
    const float radius = 0.08f;
    const float restitution = 0.2f;

    for (int i = 0; i < positions.size(); i++)
    {
        if (positions[i].y < radius)
        {
            positions[i].y = radius;

            // Reverse downward velocity with some energy loss
            if (velocities[i].y < 0.0f)
            {
                velocities[i].y *= -restitution;
            }

            // Reduce horizontal velocity through friction
            velocities[i].x *= 0.98f;
            velocities[i].z *= 0.98f;
        }
    }
}

void Scene4::simulateStep()
{
    int n = points.size();

    // Store the current state
    std::vector<glm::vec3> positions(n);
    std::vector<glm::vec3> velocities(n);

    for (int i = 0; i < n; i++)
    {
        positions[i] = points[i].position;
        velocities[i] = points[i].velocity;
    }

    // Calculate accelerations at the beginning
    std::vector<glm::vec3> accelerations =
        computeAccelerations(positions);

    if (method == 0) // Euler
    {
        for (int i = 0; i < n; i++)
        {
            positions[i] += dt * velocities[i];
            velocities[i] += dt * accelerations[i];
        }
    }
    else if (method == 1) // Midpoint
    {
        // Estimate the state at the midpoint
        std::vector<glm::vec3> midPositions(n);
        std::vector<glm::vec3> midVelocities(n);

        for (int i = 0; i < n; i++)
        {
            midPositions[i] =
                positions[i] + 0.5f * dt * velocities[i];

            midVelocities[i] =
                velocities[i] + 0.5f * dt * accelerations[i];
        }

        // Handle collisions at the estimated midpoint
        resolveGroundCollision(midPositions, midVelocities);

        // Calculate accelerations at the midpoint
        std::vector<glm::vec3> midAccelerations =
            computeAccelerations(midPositions);

        // Update using midpoint values
        for (int i = 0; i < n; i++)
        {
            positions[i] += dt * midVelocities[i];
            velocities[i] += dt * midAccelerations[i];
        }
    }

    // Resolve collisions after the integration step
    resolveGroundCollision(positions, velocities);

    // Store the updated state
    for (int i = 0; i < n; i++)
    {
        points[i].position = positions[i];
        points[i].velocity = velocities[i];
    }
}

void Scene4::onDraw(Renderer& renderer)
{
    // Draw the ground
    renderer.drawWireCube(
        glm::vec3(0.0f, -0.05f, 0.0f),
        glm::vec3(5.0f, 0.05f, 5.0f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );

    // Draw springs
    for (const Spring& spring : springs)
    {
        glm::vec3 a = points[spring.pointA].position;
        glm::vec3 b = points[spring.pointB].position;

        renderer.drawLine(a, b, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // Draw mass points
    for (const MassPoint& point : points)
    {
        renderer.drawSphere(
            point.position,
            0.08f,
            glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
        );
    }
}

void Scene4::onGUI()
{
    const char* methods[] = { "Euler", "Midpoint" };

    ImGui::Combo(
        "Integration Method",
        &method,
        methods,
        2
    );

    ImGui::SliderFloat(
        "Time step",
        &dt,
        0.0001f,
        0.02f,
        "%.4f"
    );

    ImGui::SliderFloat(
        "Gravity",
        &gravity,
        0.0f,
        20.0f
    );

    if (ImGui::Button("Reset Simulation"))
    {
        resetSimulation();
    }

    ImGui::Separator();

    ImGui::Text("Mass point interaction");

    ImGui::SliderInt(
        "Selected point",
        &selectedPoint,
        0,
        static_cast<int>(points.size()) - 1
    );

    if (ImGui::Button("Apply upward impulse"))
    {
        points[selectedPoint].velocity.y += 2.5f;
    }
}




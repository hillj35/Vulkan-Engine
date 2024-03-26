#pragma once

#include "lve_window.hpp"
#include <glm/glm.hpp>

#include <chrono>

namespace lve {
class Camera {
public:
    Camera(GLFWwindow *window);
    ~Camera();

    void HandleInput();
    void Move();
    void ShowParameterGui();
    glm::mat4 GetViewMatrix();

private:
    GLFWwindow *window;

    glm::vec3 position;
    glm::vec3 lookVector;
    glm::vec2 movementVector;

    float movementSpeed = 5.0f;
    float mouseSensitivity = 15.0f;

    double prevMousePosX = 0.0, prevMousePosY = 0.0;
    double curMousePosX = 0.0, curMousePosY = 0.0;
    bool mouseButtonHeld = false;

    std::chrono::_V2::high_resolution_clock::time_point previousTime = std::chrono::_V2::high_resolution_clock::time_point::min();
};
} // namespace lve
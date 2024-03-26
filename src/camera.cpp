#include "camera.hpp"
#include "imgui.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>

namespace lve {
Camera::Camera(GLFWwindow *window) : window{window} {
    position = {0.0f, 0.0f, 0.0f};
    lookVector = {1.0f, 0.0f, 0.0f};
    movementVector = {0.0f, 0.0f};
};
Camera::~Camera(){};

void Camera::HandleInput() {
    // forward and back movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        movementVector.y = 1.0;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        movementVector.y = -1.0f;
    } else {
        movementVector.y = 0.0f;
    }

    // left and right movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        movementVector.x = -1.0f;
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        movementVector.x = 1.0f;
    } else {
        movementVector.x = 0.0f;
    }
    if (glm::length(movementVector) > 0) {
        movementVector = glm::normalize(movementVector);
    }

    // mouse input
    if (mouseButtonHeld != glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        mouseButtonHeld = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        mouseButtonHeld ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED)
                        : glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    prevMousePosX = curMousePosX;
    prevMousePosY = curMousePosY;
    glfwGetCursorPos(window, &curMousePosX, &curMousePosY);
}

void Camera::Move() {
    if (previousTime == std::chrono::_V2::high_resolution_clock::time_point::min()) {
        previousTime = std::chrono::high_resolution_clock::now();
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();

    previousTime = currentTime;

    if (mouseButtonHeld) {
        // rotate look vector by delta mouse position
        double deltaX = prevMousePosX - curMousePosX;
        double deltaY = prevMousePosY - curMousePosY;

        lookVector = glm::rotate(lookVector, (float)glm::radians(deltaY * mouseSensitivity * deltaTime),
                                 glm::cross(glm::normalize(lookVector), {0.0f, 0.0f, 1.0f}));
        lookVector = glm::rotate(lookVector, (float)glm::radians(deltaX * mouseSensitivity * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    position += glm::normalize(lookVector) * movementVector.y * movementSpeed * deltaTime;
    position += glm::cross(glm::normalize(lookVector), {0.0f, 0.0f, 1.0f}) * movementVector.x * movementSpeed * deltaTime;
}

glm::mat4 Camera::GetViewMatrix() { return glm::lookAt(position, position + lookVector, {0.0, 0.0, 1.0}); }

void Camera::ShowParameterGui() {
    ImGui::Begin("Camera Details");
    ImGui::SeparatorText("Movement");
    ImGui::DragFloat("Movement Speed", &movementSpeed, 0.01f, 0.0f, FLT_MAX, "%.3f", 0);
    ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity, 0.01f, 0.0f, FLT_MAX, "%.3f", 0);

    ImGui::SeparatorText("Camera Position");
    ImGui::DragFloat3("Position", (float *)&position, 0.01f, FLT_MIN, FLT_MAX, "%.3f", 0);
    ImGui::DragFloat3("Look Vector", (float *)&lookVector, 0.01f, FLT_MIN, FLT_MAX, "%.3f", 0);
    ImGui::DragFloat2("Movement Vector", (float *)&movementVector, 0.01f, FLT_MIN, FLT_MAX, "%.3f", 0);
    ImGui::End();
}
} // namespace lve
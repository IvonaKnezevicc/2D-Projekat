#include "../Header/Camera.h"
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch), fov(45.0f),
      movementSpeed(2.5f), mouseSensitivity(0.1f), boundsSet(false)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(int direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    glm::vec3 newPosition = position;
    
    if (direction == 0) {
        newPosition += front * velocity;
    }
    if (direction == 1) {
        newPosition -= front * velocity;
    }
    if (direction == 2) {
        newPosition -= right * velocity;
    }
    if (direction == 3) {
        newPosition += right * velocity;
    }
    
    if (boundsSet) {
        newPosition.x = std::max(minBounds.x, std::min(maxBounds.x, newPosition.x));
        newPosition.y = std::max(minBounds.y, std::min(maxBounds.y, newPosition.y));
        newPosition.z = std::max(minBounds.z, std::min(maxBounds.z, newPosition.z));
    }
    
    position = newPosition;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    fov -= yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::setBounds(glm::vec3 minBounds, glm::vec3 maxBounds) {
    this->minBounds = minBounds;
    this->maxBounds = maxBounds;
    boundsSet = true;
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

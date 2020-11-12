#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "camera.h"


Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) :  cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
                                                                                cameraSpeed(defaultCameraSpeed),
                                                                                cameraSensitivity(defaultCameraSensitivity),
                                                                                cameraFOV(defaultCameraFOV)
{
    this->cameraPosition = position;
    this->worldUp = up;
    this->cameraYaw = yaw;
    this->cameraPitch = pitch;
    this->updateCameraVectors();
}


Camera::~Camera()
{

}


glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->cameraPosition, this->cameraPosition + this->cameraFront, this->cameraUp);
}


void Camera::keyboardCall(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat cameraVelocity = this->cameraSpeed * deltaTime;

    if (direction == FORWARD)
        this->cameraPosition += this->cameraFront * cameraVelocity;
    if (direction == BACKWARD)
        this->cameraPosition -= this->cameraFront * cameraVelocity;
    if (direction == LEFT)
        this->cameraPosition -= this->cameraRight * cameraVelocity;
    if (direction == RIGHT)
        this->cameraPosition += this->cameraRight * cameraVelocity;
}


void Camera::mouseCall(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= this->cameraSensitivity;
    yoffset *= this->cameraSensitivity;
    this->cameraYaw += xoffset;
    this->cameraPitch += yoffset;

    if (constrainPitch)
    {
        if (this->cameraPitch > 89.0f)
            this->cameraPitch = 89.0f;
        if (this->cameraPitch < -89.0f)
            this->cameraPitch = -89.0f;
    }

    this->updateCameraVectors();
}


void Camera::scrollCall(GLfloat yoffset)
{
    if (this->cameraFOV >= glm::radians(1.0f) && this->cameraFOV <= glm::radians(45.0f))
        this->cameraFOV -= glm::radians(yoffset);
    if (this->cameraFOV <= glm::radians(1.0f))
        this->cameraFOV = glm::radians(1.0f);
    if (this->cameraFOV >= glm::radians(45.0f))
        this->cameraFOV = glm::radians(45.0f);
}


void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->cameraYaw)) * cos(glm::radians(this->cameraPitch));
    front.y = sin(glm::radians(this->cameraPitch));
    front.z = sin(glm::radians(this->cameraYaw)) * cos(glm::radians(this->cameraPitch));

    this->cameraFront = glm::normalize(front);
    this->cameraRight = glm::normalize(glm::cross(this->cameraFront, this->worldUp));
    this->cameraUp = glm::normalize(glm::cross(this->cameraRight, this->cameraFront));
}

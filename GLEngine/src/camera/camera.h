#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>


const GLfloat defaultCameraYaw = -90.0f;
const GLfloat defaultCameraPitch = 0.0f;
const GLfloat defaultCameraSpeed = 4.0f;
const GLfloat defaultCameraSensitivity = 0.10f;
const GLfloat defaultCameraFOV = glm::radians(45.0f);

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};


class Camera
{
    public:
        glm::vec3 cameraPosition;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        glm::vec3 cameraRight;
        glm::vec3 worldUp;
        GLfloat cameraYaw;
        GLfloat cameraPitch;
        GLfloat cameraSpeed;
        GLfloat cameraSensitivity;
        GLfloat cameraFOV;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = defaultCameraYaw, GLfloat pitch = defaultCameraPitch);
        ~Camera();
        glm::mat4 GetViewMatrix();
        void keyboardCall(Camera_Movement direction, GLfloat deltaTime);
        void mouseCall(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
        void scrollCall(GLfloat yoffset);

    private:
        void updateCameraVectors();
};

#endif

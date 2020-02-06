#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Default camera values
const float defaultYaw = -90.0f;
const float defaultPitch = 0.0f;
const float defaultRoll = 0.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Origin;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    float Roll;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float inYaw = defaultYaw, float inPitch = defaultPitch, float inRoll = defaultRoll) : Front(glm::vec3(0.0f, 0.0f, -1.0f))
    {
        Position = position;
        Origin = position;
        WorldUp = up;
        Yaw = inYaw;
        Pitch = inPitch;
        Roll = inRoll;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void TranslateCamera(float posX, float posY, float posZ) {
        Position = Origin + glm::vec3(posX, posY, posZ);
    }

    void RotateCamera(float inYaw, float inPitch, float inRoll) {
        Yaw = inYaw;
        Pitch = inPitch;
        Roll = inRoll;
        updateCameraVectors();
    }

    void setNewOrigin(float posX, float posY, float posZ) {
        Origin = glm::vec3(posX, posY, posZ);
    }

private:
    // Calculates front, right, and up vectors from the yaw, pitch, and roll values.
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
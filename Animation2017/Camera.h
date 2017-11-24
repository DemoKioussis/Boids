#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "Direction.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods


// Default camera values
const float YAW        =  90.0f;
const float PITCH      =  0.0f;
const float SENSITIVTY =  0.1f;
const float ZOOM       =  45.0f;
const float SPEED_FAST = 500.0f;
const float SPEED_SLOW = 100.0f;
const float SPEED = SPEED_FAST;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
	WindowManager* windowManager;
	bool fps_mode_enabled;
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Eular Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
	
    Camera(WindowManager* windowManager, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(1.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM), windowManager(windowManager), fps_mode_enabled(false)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(WindowManager* windowManager, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM), windowManager(windowManager), fps_mode_enabled(false)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
	void changeSpeed() {
		if (MovementSpeed == SPEED_FAST)
			MovementSpeed = SPEED_SLOW;
		else
			MovementSpeed = SPEED_FAST;
	}
    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(DIRECTION direction,bool isPaused)
    {
		if (!fps_mode_enabled || !glfwGetWindowAttrib(windowManager->getWindow(), GLFW_FOCUSED))
		{
			return;
		}
		
		float velocity = -MovementSpeed*TimeSystem::getFrameDeltaTime();

        if (direction == FORWARD)
            Position -= Front * velocity;
        if (direction == BACKWARD)
            Position += Front * velocity;
        if (direction == LEFT)
            Position += Right * velocity;
        if (direction == RIGHT)
            Position -= Right * velocity;
		if (direction == UP)
			Position -= WorldUp * velocity;
		if (direction == DOWN)
			Position += WorldUp * velocity;
    }

	void ToggleFpsMode() 
	{
		fps_mode_enabled = !fps_mode_enabled;

		if (fps_mode_enabled)
		{
			glfwSetInputMode(windowManager->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(windowManager->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
		if (!fps_mode_enabled || !glfwGetWindowAttrib(windowManager->getWindow(), GLFW_FOCUSED))
		{
			return;
		}

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Eular angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }
	void translate(glm::vec3 p) {
		Position += p;
	}

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
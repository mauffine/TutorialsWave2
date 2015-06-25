#include "FlyCamera.h"
#include <GLFW\glfw3.h>

#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

void FlyCamera::Update(double dt)
{
	HandleKeyboardInput(dt);
	HandleMouseInput(dt);
}

void FlyCamera::HandleKeyboardInput(double dt)
{
	//Get the cameras forward/up/right
	glm::vec3 vRight = m_worldTransform[0].xyz;
	glm::vec3 vUp = m_worldTransform[1].xyz;
	glm::vec3 vForward = m_worldTransform[2].xyz;

	glm::vec3 moveDir(0.0f);

	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir -= vForward;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir += vForward;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir -= vRight;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir += vRight;
	}

	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
	}

	float fLength = glm::length(moveDir);
	if (fLength > 0.01f)
	{
		moveDir = ((float)dt * m_fFlySpeed) * glm::normalize(moveDir);
		SetPosition(GetPosition() + moveDir);
	}
}

void FlyCamera::HandleMouseInput(double dt)
{
	if (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (m_bViewButtonClicked == false)
		{
			int width, height;
			glfwGetFramebufferSize(m_pWindow, &width, &height);


			m_dCursorX = width / 2.0;
			m_dCursorY = height / 2.0;
			glfwSetCursorPos(m_pWindow, m_dCursorX, m_dCursorY);
			m_bViewButtonClicked = true;
		}
		else
		{
			double mouseX, mouseY;
			glfwGetCursorPos(m_pWindow, &mouseX, &mouseY);

			glm::vec2 offset = glm::vec2(mouseX - m_dCursorX, mouseY - m_dCursorY);

			m_dCursorX = mouseX;
			m_dCursorY = mouseY;

			CalculateRotation(offset, dt);

			std::cout << offset.x << " " << offset.y << std::endl;
			UpdateProjectionViewTransform();
		}
	}
	else
	{
		m_bViewButtonClicked = false;
	}
}

void FlyCamera::CalculateRotation(glm::vec2 a_offset, double dt)
{
	glm::vec3 upVec = glm::vec3(0, 1, 0); 
	glm::vec3 rightVec = glm::vec3(1,0,0);

	glm::normalize(upVec);
	glm::normalize(rightVec);

	glm::mat4 xRot = glm::rotate(a_offset.x * (float)dt * 0.1f, upVec);
	glm::mat4 yRot = glm::rotate(a_offset.y * (float)dt * 0.1f, rightVec);

	if (glm::length(a_offset) > 0.001f)
	{
		m_worldTransform = m_worldTransform * xRot;
		m_worldTransform = m_worldTransform * yRot;
	}
	
	//fix them weird vectors
	rightVec = glm::normalize(glm::cross(glm::vec3(0,1,0), glm::vec3(m_worldTransform[2])));
	upVec = glm::normalize(glm::cross(glm::vec3(m_worldTransform[2]), rightVec));

	m_worldTransform[2] = glm::vec4(glm::normalize(glm::vec3(m_worldTransform[2])), 0);
	glm::normalize(upVec);
	glm::normalize(rightVec);

	m_worldTransform[0] = glm::vec4(rightVec,0);
	m_worldTransform[1] = glm::vec4(upVec, 0);
	UpdateProjectionViewTransform();

}
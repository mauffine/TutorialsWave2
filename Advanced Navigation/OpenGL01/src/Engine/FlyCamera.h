#ifndef _FLY_CAMERA_H_
#define _FLY_CAMERA_H_

#include <Engine/BaseCamera.h>
#include <GLFW\glfw3.h>

class FlyCamera : public BaseCamera
{
public:
	FlyCamera()
	{
		m_fFlySpeed = 10.f;
		m_bViewButtonClicked = false;
	}
	FlyCamera(float flySpeed) : BaseCamera(), m_fFlySpeed(flySpeed)
	{
		
	}
	~FlyCamera() {};

	virtual void Update(double dt);

	void SetFlySpeed(float fSpeed);
	float GetFlySpeed() const { return m_fFlySpeed;  }

	//TEMP - Pass the window to be used to query the keyboard
	//This will get changed to an Input Manager later
	void SetInputWindow(GLFWwindow* pWindow) { m_pWindow = pWindow; }


protected:
	void HandleKeyboardInput(double dt);
	void HandleMouseInput(double dt);

	void CalculateRotation(glm::vec2 a_offset, double dt);

	GLFWwindow* m_pWindow;
	float m_fFlySpeed;

	bool m_bViewButtonClicked;
	double m_dCursorX, m_dCursorY;

};

#endif
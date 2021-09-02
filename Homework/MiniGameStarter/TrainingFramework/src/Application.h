#pragma once
#include "Singleton.h"
#include "GameConfig.h"
#include "Camera.h"

#include "Utils/InputManager.h"

class Application : public CSingleton<Application>
{
public:
	Application();
	~Application();

	void	Init();
	void	Update(GLfloat deltaTime);
	void	Render();
	void	HandleKeyEvent(unsigned char key, bool bIsPressed);
	void	HandleTouchEvent(GLint x, GLint y, bool bIsPressed);
	void	HandleMouseMoveEvent(GLint x, GLint y);
	void	Exit();
	std::shared_ptr<Camera> GetCamera() {
		return m_camera;
	}

	const InputManager* GetInputManager() const
	{
		return &m_inputManager;
	}

private:
	std::shared_ptr<Camera> m_camera;
	InputManager m_inputManager;
};


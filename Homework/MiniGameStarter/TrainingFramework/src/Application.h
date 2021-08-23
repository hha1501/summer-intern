#pragma once
#include "Singleton.h"
#include "GameConfig.h"
#include "Camera.h"


class Application : public CSingleton<Application>
{
public:
	Application();
	~Application();

	void	Init();
	void	Update(GLfloat deltaTime);
	void	Render();
	void	HandleKeyEvent(unsigned char key, bool bIsPresseded);
	void	HandleTouchEvent(GLint x, GLint y, bool bIsPresseded);
	void	HandleMouseMoveEvent(GLint x, GLint y);
	void	Exit();
	std::shared_ptr<Camera> GetCamera() {
		return m_camera;
	}

private:
	std::shared_ptr<Camera> m_camera;
};


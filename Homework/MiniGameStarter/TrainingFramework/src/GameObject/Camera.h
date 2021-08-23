#pragma once

#include "utilities.h" 
#include "GameConfig.h"


class Camera
{
public:
	Camera(void);
	Camera::Camera(GLint id, Vector3 position, Vector3 target, Vector3 up, GLfloat fovY, GLfloat fNear, GLfloat fFar, GLfloat fSpeed);
	Camera::Camera(GLint id, GLfloat left, GLfloat right, GLfloat top, GLfloat bottom, GLfloat fNear, GLfloat fFar, GLfloat fSpeed);
	~Camera(void);
	Matrix		GetLookAtCameraMatrix();
	Vector3		GetPosition();
	void 		SetTarget(Vector3 target);
	void		SetPosition(Vector3 pos);

	GLfloat		GetNear();
	GLfloat		GetFar();
	void		SetId(GLint id) { m_id = id; }
	GLint			GetId() { return m_id; }
	Matrix		GetWorldMatrix();
	Matrix		GetViewMatrix();
	Matrix		GetProjecttionMatrix();
	void		Update(GLfloat deltaTime);
	void		MoveUp(GLfloat deltaTime);
	void		MoveLeft(GLfloat deltaTime);

private:
	GLint		m_id;
	bool	m_is2D;
	Vector3		m_position;
	Vector3		m_target;
	Vector3		m_up;

	Matrix		m_worldMatrix;
	Matrix		m_viewMatrix;
	Matrix		m_projectionMatrix;

	GLfloat		m_fSpeed;
	GLfloat		m_fNear;
	GLfloat		m_fFar;
	bool	m_isChanged;

	void		CalculateWorldMatrix();
	void		CalculateViewMatrix();
};


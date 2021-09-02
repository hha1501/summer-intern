#include "GameButton.h"

#include "Camera.h"

GameButton::GameButton(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture)
	: Sprite2D(-1, model, shader, texture), m_pBtClick(nullptr), m_isHolding(false)
{
}

GameButton::~GameButton()
{
}

void GameButton::SetOnClick(void(*pBtClickFun)())
{
	m_pBtClick = pBtClickFun;
}

bool GameButton::HandleTouchEvents(GLint x, GLint y, bool bIsPressed)
{
	bool isHandled = false;
	if (bIsPressed)
	{
		if (TestPointInBounds(x, y))
		{
			// The button is being pressed down
			m_isHolding = true;
		}
	}
	else
	{
		if (TestPointInBounds(x, y))
		{
			// Only perform click action when the same button was pressed down and released
			m_pBtClick();
			isHandled = true;
		}
		m_isHolding = false;
	}

	return isHandled;
}

bool GameButton::IsHolding() const
{
	return m_isHolding;
}

bool GameButton::TestPointInBounds(GLint x, GLint y) const
{
	Matrix lookAtMat = m_pCamera->GetLookAtMatrix();

	Vector4 buttonTopLeft(m_position.x - m_width / 2, m_position.y - m_height / 2, 0.0f, 1.0f);
	Vector4 buttonBottomRight(m_position.x + m_width / 2, m_position.y + m_height / 2, 0.0f, 1.0f);

	Vector4 topLeftNDC = buttonTopLeft * lookAtMat;
	Vector4 bottomRightNDC = buttonBottomRight * lookAtMat;

	Vector2 pointNDC(x * 2.0f / (float)Globals::screenWidth - 1.0f,
					 -y * 2.0f / (float)Globals::screenHeight + 1.0f);

	return (pointNDC.x > topLeftNDC.x && pointNDC.x < bottomRightNDC.x)
		&& (pointNDC.y < topLeftNDC.y && pointNDC.y > bottomRightNDC.y);
}

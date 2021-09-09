#include "GSPlay.h"

#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Camera.h"
#include "Font.h"
#include "Sprite2D.h"
#include "Sprite3D.h"
#include "Text.h"
#include "GameButton.h"
#include "Application.h"

#include "../Game/Map/GameMap.h"

constexpr float c_victoryWaitTime = 2.0f;

GSPlay::GSPlay() : m_inVictoryScreen(false), m_victoryWaitTime{}
{
    m_inputManager = Application::GetInstance()->GetInputManager();
    m_sessionManager = Application::GetInstance()->GetSessionManager();
}

void GSPlay::Init()
{
    auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D.nfg");
    auto texture = ResourceManagers::GetInstance()->GetTexture("game_background.tga");

    // background
    auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
    m_background = std::make_unique<Sprite2D>(model, shader, texture);
    m_background->Set2DPosition(Vector2((float)Globals::screenWidth / 2, (float)Globals::screenHeight / 2));
    m_background->SetSize((float)Globals::screenWidth, (float)Globals::screenHeight);

    // button close
    texture = ResourceManagers::GetInstance()->GetTexture("button_close.tga");
    std::unique_ptr<GameButton> closeButton = std::make_unique<GameButton>(model, shader, texture);
    closeButton->Set2DPosition(Globals::screenWidth - 30, 30);
    closeButton->SetSize(50, 50);
    closeButton->SetOnClick([]()
    {
        GameStateMachine::GetInstance()->PopState();
    });
    m_listButton.push_back(std::move(closeButton));

    // button restart
    texture = ResourceManagers::GetInstance()->GetTexture("button_restart.tga");
    std::unique_ptr<GameButton> restartButton = std::make_unique<GameButton>(model, shader, texture);
    restartButton->Set2DPosition(30, 30);
    restartButton->SetSize(50, 50);
    restartButton->SetOnClick([this]()
    {
        RestartGameWorld();
    });
    m_listButton.push_back(std::move(restartButton));

    RestartGameWorld();
}

void GSPlay::Exit()
{
}


void GSPlay::Pause()
{
}

void GSPlay::Resume()
{
}


void GSPlay::HandleEvents()
{
}

void GSPlay::HandleKeyEvents(int key, bool bIsPressed)
{
}

void GSPlay::HandleTouchEvents(int x, int y, bool bIsPressed)
{
    for (auto& button : m_listButton)
    {
        if (button->HandleTouchEvents(x, y, bIsPressed))
        {
            break;
        }
    }
}

void GSPlay::HandleMouseMoveEvents(int x, int y)
{
}

void GSPlay::Update(float deltaTime)
{
    for (auto& it : m_listButton)
    {
        it->Update(deltaTime);
    }

    if (m_inputManager->KeyDown(KeyCode::G))
    {
        m_gameWorld.ToggleGravitySelection();
    }
    else if (m_inputManager->KeyDown(KeyCode::LEFT))
    {
        if (m_gameWorld.IsInGravtitySelection())
        {
            m_gameWorld.SetGravity(GameWorld::GravityDirection::Left);
        }
        else
        {
            m_gameWorld.MovePlayer(GameWorld::InputDirection::Left);
        }
    }
    else if (m_inputManager->KeyDown(KeyCode::RIGHT))
    {
        if (m_gameWorld.IsInGravtitySelection())
        {
            m_gameWorld.SetGravity(GameWorld::GravityDirection::Right);
        }
        else
        {
            m_gameWorld.MovePlayer(GameWorld::InputDirection::Right);
        }
    }

    m_gameWorld.Update(deltaTime);

    if (!m_inVictoryScreen)
    {
        if (m_gameWorld.IsVictory())
        {
            OnVictory();
        }
    }
    else
    {
        m_victoryWaitTime += deltaTime;
        if (m_victoryWaitTime >= c_victoryWaitTime)
        {
            m_inVictoryScreen = false;
            GameStateMachine::GetInstance()->PopState();
        }
    }

    // TODO: Handle game over
}

void GSPlay::Draw()
{
    m_background->Draw();

    m_gameWorld.Draw();

    for (auto& it : m_listButton)
    {
        it->Draw();
    }

    if (m_inVictoryScreen)
    {
        m_victoryPanel->Draw();
    }
}

void GSPlay::RestartGameWorld()
{
    if (m_inVictoryScreen)
    {
        return;
    }

    m_gameWorld = GameWorld();
    m_gameWorld.Init(m_sessionManager->GetSelectedLevel());
}

void GSPlay::OnVictory()
{
    auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D.nfg");
    auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");

    // victory panel
    auto texture = ResourceManagers::GetInstance()->GetTexture("panel_victory.tga");
    m_victoryPanel = std::make_unique<Sprite2D>(model, shader, texture);
    m_victoryPanel->Set2DPosition(Vector2((float)Globals::screenWidth / 2, (float)Globals::screenHeight / 2));
    m_victoryPanel->SetSize(450.0f, 180.0f);

    m_inVictoryScreen = true;
}

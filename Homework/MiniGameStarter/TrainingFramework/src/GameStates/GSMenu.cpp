#include "GSMenu.h"
#include "Camera.h"

#include "Application.h"

GSMenu::GSMenu() : GameStateBase(StateType::STATE_MENU),
m_background(nullptr), m_listButton(std::list<std::shared_ptr<GameButton>>{}), m_textGameName(nullptr)
{
}


GSMenu::~GSMenu()
{
}



void GSMenu::Init()
{
    auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D.nfg");
    auto texture = ResourceManagers::GetInstance()->GetTexture("background.tga");

    // background
    auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
    m_background = std::make_shared<Sprite2D>(model, shader, texture);
    m_background->Set2DPosition(Globals::screenWidth / 2, Globals::screenHeight / 2);
    m_background->SetSize(Globals::screenWidth, Globals::screenHeight);

    // start button
    texture = ResourceManagers::GetInstance()->GetTexture("button_start.tga");
    std::shared_ptr<GameButton> buttonStart = std::make_shared<GameButton>(model, shader, texture);
    buttonStart->Set2DPosition(Globals::screenWidth / 2, Globals::screenHeight / 2 - 30);
    buttonStart->SetSize(200, 70);
    buttonStart->SetOnClick([]()
    {
        GameStateMachine::GetInstance()->ChangeState(StateType::STATE_LEVEL_SELECT);
    });
    m_listButton.push_back(buttonStart);

    // tutorial button
    texture = ResourceManagers::GetInstance()->GetTexture("button_tutorial.tga");
    std::shared_ptr<GameButton> buttonTutorial = std::make_shared<GameButton>(model, shader, texture);
    buttonTutorial->Set2DPosition(Globals::screenWidth / 2, Globals::screenHeight / 2 + 50);
    buttonTutorial->SetSize(150, 60);
    buttonTutorial->SetOnClick([]()
    {
        // TODO: tutorial scene handler
    });
    m_listButton.push_back(buttonTutorial);

    // credits button
    texture = ResourceManagers::GetInstance()->GetTexture("button_credits.tga");
    std::shared_ptr<GameButton> buttonCredits = std::make_shared<GameButton>(model, shader, texture);
    buttonCredits->Set2DPosition(Globals::screenWidth / 2, Globals::screenHeight / 2 + 120);
    buttonCredits->SetSize(150, 60);
    buttonCredits->SetOnClick([]()
    {
        GameStateMachine::GetInstance()->ChangeState(StateType::STATE_CREDIT);
    });
    m_listButton.push_back(buttonCredits);

    // exit button
    texture = ResourceManagers::GetInstance()->GetTexture("button_close.tga");
    std::shared_ptr<GameButton> buttonExit = std::make_shared<GameButton>(model, shader, texture);
    buttonExit->Set2DPosition(Globals::screenWidth - 50, 50);
    buttonExit->SetSize(50, 50);
    buttonExit->SetOnClick([]()
    {
        exit(0);
    });
    m_listButton.push_back(buttonExit);

    // game title
    shader = ResourceManagers::GetInstance()->GetShader("TextShader");
    std::shared_ptr<Font> font = ResourceManagers::GetInstance()->GetFont("Gamer.ttf");
    m_textGameName = std::make_shared<Text>(shader, font, "an-gravi-ti", Vector4(255.0f, 93.0f, 33.0f, 255.0f) / 255.0f, 3.0f);
    m_textGameName->Set2DPosition((float)Globals::screenWidth / 2 - 130, (float)Globals::screenHeight / 2 - 100);

    // play background sound
    Application::GetInstance()->GetSoundManager()->PlayBackgroundSound();
}

void GSMenu::Exit()
{
}


void GSMenu::Pause()
{
}

void GSMenu::Resume()
{
}


void GSMenu::HandleEvents()
{
}

void GSMenu::HandleKeyEvents(int key, bool bIsPressed)
{
}

void GSMenu::HandleTouchEvents(int x, int y, bool bIsPressed)
{
    for (auto& button : m_listButton)
    {
        if (button->HandleTouchEvents(x, y, bIsPressed))
        {
            break;
        }
    }
}

void GSMenu::HandleMouseMoveEvents(int x, int y)
{
}

void GSMenu::Update(float deltaTime)
{
    m_background->Update(deltaTime);
    for (auto& it : m_listButton)
    {
        it->Update(deltaTime);
    }
}

void GSMenu::Draw()
{
    m_background->Draw();
    for (auto& it : m_listButton)
    {
        it->Draw();
    }
    m_textGameName->Draw();
}

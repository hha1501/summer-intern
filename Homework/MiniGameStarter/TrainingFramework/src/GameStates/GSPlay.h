#pragma once
#include "GameStateBase.h"

#include "../Game/GameWorld.h"

#include "../Utils/InputManager.h"
#include "../Utils/SessionManager.h"

class Sprite2D;
class Sprite3D;
class Text;
class GameButton;

class GSPlay :
    public GameStateBase
{
public:
    GSPlay();
    ~GSPlay();

    void Init() override;
    void Exit() override;

    void Pause() override;
    void Resume() override;

    void HandleEvents() override;
    void HandleKeyEvents(int key, bool bIsPressed) override;
    void HandleTouchEvents(int x, int y, bool bIsPressed) override;
    void HandleMouseMoveEvents(int x, int y) override;
    void Update(float deltaTime) override;
    void Draw() override;

private:
    void RestartGameWorld();
    void OnVictory();

private:
    std::shared_ptr<Sprite2D> m_background;
    std::list<std::shared_ptr<GameButton>> m_listButton;

    std::unique_ptr<Sprite2D> m_victoryPanel;

    GameWorld m_gameWorld;
    const InputManager* m_inputManager;
    const SessionManager* m_sessionManager;

    bool m_inVictoryScreen;
    float m_victoryWaitTime;
};


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


GSPlay::~GSPlay()
{
}

void CreateMap()
{
    constexpr uint8_t mapSize = 10;
    GameMap gameMap(mapSize, mapSize);

    std::vector <GameMap::EntityEntry>& entities = gameMap.Entities();
    entities.push_back(GameMap::EntityEntry{ 5, 1, (uint8_t)EntityType::Player });
    entities.push_back(GameMap::EntityEntry{ 1, 4, (uint8_t)EntityType::Key });

    std::vector<uint8_t>& tiles = gameMap.Tiles();
    for (size_t i = 0; i < mapSize; i++)
    {
        tiles[i + mapSize * 0] = (uint8_t)TileType::Wall;
        tiles[i + mapSize * (mapSize - 1)] = (uint8_t)TileType::Wall;
        tiles[0 + mapSize * i] = (uint8_t)TileType::Wall;
        tiles[(mapSize - 1) + mapSize * i] = (uint8_t)TileType::Wall;
    }

    tiles[(mapSize - 1) + mapSize * 1] = (uint8_t)TileType::Exit;

    tiles[1 + 10 * 1] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[2 + 10 * 1] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[3 + 10 * 1] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[1 + 10 * 2] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[2 + 10 * 2] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[1 + 10 * 3] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);

    GameMap::SaveToFile(gameMap, 2);
}

void GSPlay::Init()
{
    auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D.nfg");
    auto texture = ResourceManagers::GetInstance()->GetTexture("game_background.tga");

    // background
    auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
    m_background = std::make_shared<Sprite2D>(model, shader, texture);
    m_background->Set2DPosition(Vector2((float)Globals::screenWidth / 2, (float)Globals::screenHeight / 2));
    m_background->SetSize((float)Globals::screenWidth, (float)Globals::screenHeight);

    // button close
    texture = ResourceManagers::GetInstance()->GetTexture("button_close.tga");
    std::shared_ptr<GameButton> closeButton = std::make_shared<GameButton>(model, shader, texture);
    closeButton->Set2DPosition(Globals::screenWidth - 30, 30);
    closeButton->SetSize(50, 50);
    closeButton->SetOnClick([]()
    {
        GameStateMachine::GetInstance()->PopState();
    });
    m_listButton.push_back(closeButton);

    // button restart
    texture = ResourceManagers::GetInstance()->GetTexture("button_restart.tga");
    std::shared_ptr<GameButton> restartButton = std::make_shared<GameButton>(model, shader, texture);
    restartButton->Set2DPosition(30, 30);
    restartButton->SetSize(50, 50);
    restartButton->SetOnClick([this]()
    {
        RestartGameWorld();
    });
    m_listButton.push_back(restartButton);

    RestartGameWorld();
    //CreateMap();
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

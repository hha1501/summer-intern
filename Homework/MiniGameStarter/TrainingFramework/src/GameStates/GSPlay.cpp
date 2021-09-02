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

GSPlay::GSPlay()
{
}


GSPlay::~GSPlay()
{
}

void CreateMap()
{
    constexpr uint8_t mapSize = 10;
    GameMap gameMap(mapSize, mapSize);

    std::vector <GameMap::EntityEntry>& entities = gameMap.Entities();
    entities.push_back(GameMap::EntityEntry{ 1, 1, (uint8_t)EntityType::Player });
    entities.push_back(GameMap::EntityEntry{ 4, 6, (uint8_t)EntityType::Box });
    entities.push_back(GameMap::EntityEntry{ 5, 6, (uint8_t)EntityType::Box });
    entities.push_back(GameMap::EntityEntry{ 6, 6, (uint8_t)EntityType::Box });
    entities.push_back(GameMap::EntityEntry{ 3, 1, (uint8_t)EntityType::Rock });
    entities.push_back(GameMap::EntityEntry{ 4, 1, (uint8_t)EntityType::Rock });
    entities.push_back(GameMap::EntityEntry{ 5, 1, (uint8_t)EntityType::Key });

    std::vector<uint8_t>& tiles = gameMap.Tiles();
    for (size_t i = 0; i < mapSize; i++)
    {
        tiles[i + mapSize * 0] = (uint8_t)TileType::Wall;
        tiles[i + mapSize * (mapSize - 1)] = (uint8_t)TileType::Wall;
        tiles[0 + mapSize * i] = (uint8_t)TileType::Wall;
        tiles[(mapSize - 1) + mapSize * i] = (uint8_t)TileType::Wall;
    }

    tiles[(mapSize - 1) + mapSize * 1] = (uint8_t)TileType::Exit;

    tiles[3 + 10 * 5] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[4 + 10 * 5] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[5 + 10 * 5] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[6 + 10 * 5] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[2 + 10 * 6] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);
    tiles[7 + 10 * 6] = (uint8_t)TileType::Wall | ((uint8_t)1 << GameMap::c_tileDetailsMaskShift);

    GameMap::SaveToFile(gameMap, 1);
}

void GSPlay::Init()
{
    auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D.nfg");
    auto texture = ResourceManagers::GetInstance()->GetTexture("background_white.tga");

    // background
    auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
    m_background = std::make_shared<Sprite2D>(model, shader, texture);
    m_background->Set2DPosition(Vector2((float)Globals::screenWidth / 2, (float)Globals::screenHeight / 2));
    m_background->SetSize((float)Globals::screenWidth, (float)Globals::screenHeight);

    // button close
    texture = ResourceManagers::GetInstance()->GetTexture("button_close.tga");
    std::shared_ptr<GameButton>  button = std::make_shared<GameButton>(model, shader, texture);
    button->Set2DPosition(Globals::screenWidth - 50, 50);
    button->SetSize(50, 50);
    button->SetOnClick([]()
    {
        GameStateMachine::GetInstance()->PopState();
    });
    m_listButton.push_back(button);

    m_gameWorld.Init();
    //CreateMap();

    m_inputManager = Application::GetInstance()->GetInputManager();
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

    if (m_inputManager->KeyDown(KeyCode::W))
    {
        m_gameWorld.SetGravity(GameWorld::GravityDirection::Up);
    }
    else if (m_inputManager->KeyDown(KeyCode::A))
    {
        m_gameWorld.SetGravity(GameWorld::GravityDirection::Left);
    }
    else if (m_inputManager->KeyDown(KeyCode::S))
    {
        m_gameWorld.SetGravity(GameWorld::GravityDirection::Down);
    }
    else if (m_inputManager->KeyDown(KeyCode::D))
    {
        m_gameWorld.SetGravity(GameWorld::GravityDirection::Right);
    }

    if (m_inputManager->KeyDown(KeyCode::LEFT))
    {
        m_gameWorld.MovePlayer(GameWorld::InputDirection::Left);
    }
    else if (m_inputManager->KeyDown(KeyCode::RIGHT))
    {
        m_gameWorld.MovePlayer(GameWorld::InputDirection::Right);
    }

    m_gameWorld.Update(deltaTime);
}

void GSPlay::Draw()
{
    m_background->Draw();

    m_gameWorld.Draw();

    for (auto& it : m_listButton)
    {
        it->Draw();
    }
}
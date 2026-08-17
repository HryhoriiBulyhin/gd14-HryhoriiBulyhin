#include "pch.h"
#include "Game.h"
#include "Texture.h"
#include "utils.h"
#include "LevelPositioner.h"
#include "Level.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Mouse.h"
#include "Enemy.h"
#include "DroppedItem.h"
#include "LevelManager.h"
#include "GameSoundController.h"
#include "SoundManager.h"

#include <iostream>

Game::Game(const Window& window)
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	SDL_ShowCursor(SDL_DISABLE);

	const std::string
		visualConfigPath{ "Entity/visualConfig.json" };
	m_pResourceManager = new ResourceManager{};

	m_pGameSoundContoller = new GameSoundController{};

	m_VisualConfigManager.Load(visualConfigPath);
	m_pLevelManager = new LevelManager{ m_pResourceManager,&m_VisualConfigManager };

	m_pLevelManager->LoadFirstLevel();
	m_pCamera = new Camera
	{
		GetViewPort().width,
		GetViewPort().height
	};
	m_pMouse = new Mouse
	{
		m_VisualConfigManager.GetMouseVisualInfo(),
		m_pResourceManager
	};
	m_pCamera->Follow(m_pLevelManager->GetPlayer()->GetPosition());
}

void Game::Cleanup()
{
	delete m_pLevelManager;
	m_pLevelManager = nullptr;

	delete m_pCamera;
	m_pCamera = nullptr;

	delete m_pMouse;
	m_pMouse = nullptr;

	delete m_pGameSoundContoller;
	m_pGameSoundContoller = nullptr;

	delete m_pResourceManager;
	m_pResourceManager = nullptr;
}

void Game::Update(float elapsedSec)
{
	FPS(elapsedSec);

	Level* pLevel{ m_pLevelManager->GetLevel() };
	Player* pPlayer{ m_pLevelManager->GetPlayer() };

	const std::vector<Enemy*>& enemies{ m_pLevelManager->GetEnemies() };
	const Vector2f previousPlayerPos{ pPlayer->GetPosition() };
	pPlayer->Update(elapsedSec);

	m_pMouse->Update(GetViewPort(), m_pCamera->GetPosition(), elapsedSec);

	pPlayer->SetTargetCrosshair(m_pMouse->GetWorldPos());

	pLevel->ResolveEntityMovement(*pPlayer, previousPlayerPos);

	for (Enemy* pEnemy : enemies)
	{
		const Vector2f previousEnemyPos{ pEnemy->GetPosition() };
		pEnemy->SetTargetPlayer(pPlayer);
		pEnemy->Update(elapsedSec);

		pLevel->ResolveEntityMovement(*pEnemy, previousEnemyPos);
	}
	pLevel->Update(elapsedSec);

	m_pGameSoundContoller->Update(pPlayer, enemies, pLevel);

	if (m_pLevelManager->TryChangeLevel())
	{
		Player* pNewPlayer{ m_pLevelManager->GetPlayer() };
		m_pCamera->Follow(pNewPlayer->GetPosition());

		return;
	}
	m_pCamera->Follow(pPlayer->GetPosition());
}

void Game::Draw() const
{
	ClearBackground();
	Level* pLevel{ m_pLevelManager->GetLevel() };
	Player* pPlayer{ m_pLevelManager->GetPlayer() };
	const std::vector<Enemy*>& enemies{ m_pLevelManager->GetEnemies() };

	m_pCamera->BeginDraw();

	pLevel->Draw();

	pLevel->DrawDroppedItems();
	if (pPlayer->IsAlive())
	{
		for (Enemy* pEnemy : enemies)
		{
			pEnemy->Draw();
		}
		pPlayer->Draw();
	}
	else
	{
		// Dead player
		pPlayer->Draw();

		for (Enemy* pEnemy : enemies)
		{
			pEnemy->Draw();
		}
	}
	
	pLevel->DrawCar();
	pLevel->DrawDoors();
	m_pCamera->EndDraw();

	m_pMouse->Draw();
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	m_pLevelManager->GetPlayer()->KeyboardHandling(e);
	m_pLevelManager->ApplyRestart(e);
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	m_pLevelManager->GetPlayer()->MouseHandling(e);
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::FPS(float elapsedSec)
{
	m_FPSAccumulatedTime += elapsedSec;
	m_FPSFrames++;

	if (m_FPSAccumulatedTime >= 1.0f)
	{
		m_CurrentAvarageFPS = m_FPSFrames;
		m_FPSFrames = 0;
		m_FPSAccumulatedTime -= 1.0f;

		system("CLS");
		std::cout << "FPS: " << m_CurrentAvarageFPS << '\n';
	}
}
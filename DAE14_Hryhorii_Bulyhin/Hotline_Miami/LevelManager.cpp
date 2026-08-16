#include "pch.h"
#include "LevelManager.h"
#include "Level.h"
#include "Player.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "VisualConfigManager.h"
#include "Car.h"

LevelManager::LevelManager(ResourceManager* pResources, VisualConfigManager* pVisualConfig)
	:
	m_pResources{ pResources },
	m_pVisualConfig{ pVisualConfig },
	m_PlayerWeapon{ WeaponType::Unarmed },
	m_Level02Completed{}
{
	const std::string playerhitboxPath{ "Entity/Player/Hitboxes/PlayerHitboxes.json" },
		enemyHitboxPath{ "Entity/Enemy/Hitboxes/EnemyHitboxes.json" };

	m_PlayerHitBoxConfig = JsonImporter::ImoprtHitboxConfig(playerhitboxPath);
	m_EnemyHitBoxConfig = JsonImporter::ImoprtHitboxConfig(enemyHitboxPath);
}

LevelManager::~LevelManager()
{
	ClearLevel();
}

void LevelManager::LoadLevel(const std::string& levelPath)
{
	ClearLevel();

	m_pLevel = new Level{ levelPath };

	const bool loadEnemies{ !(m_CurrentLevel == 1 && m_Level01State.completed) };

	m_pLevel->LoadEntities(
		m_Enemies,
		m_EnemyHitBoxConfig.enemy,
		m_pVisualConfig->GetEnemyVisualInfo(),
		m_pResources,
		loadEnemies
	);

	const JsonImporter::GameData& levelData{ m_pLevel->GetData() };

	if (levelData.hasPlayerSpawn)
	{
		m_pPlayer = new Player
		{
			levelData.playerSpawn,
			m_PlayerHitBoxConfig.player,
			m_pVisualConfig->GetPlayerVisualInfo(),
			m_pResources
		};
		m_pPlayer->SetWeaponType(m_PlayerWeapon);
		m_pLevel->SetPlayer(m_pPlayer);
	}
	const bool isReturned{ m_CurrentLevel == 1 && m_Level01State.completed };

	if (isReturned)
	{
		m_pPlayer->SetPosition(m_Level01State.returnPosition);
		m_pPlayer->RefreshHitbox();
		m_pLevel->LoadCorpses(m_Level01State.corpses);
	}
	if (m_pLevel->GetCar() != nullptr)
	{
		m_pLevel->GetCar()->SetActive(m_Level02Completed);
	}
}

void LevelManager::LoadFirstLevel()
{
	m_CurrentLevel = 1;
	std::string firstLvlJsonPath{ "Levels/Level01.json" };

	LoadLevel(firstLvlJsonPath);
}

bool LevelManager::TryChangeLevel()
{
	if (m_pLevel == nullptr || m_pPlayer == nullptr)
	{
		return false;
	}
	if (!m_pLevel->IsPlayerOnLevelExit())
	{
		return false;
	}
	SaveCurrentLevel();

	if (m_CurrentLevel == 1)
	{
		std::string secondLevelPath{ "Levels/Level02.json" };
		m_CurrentLevel = 2;
		LoadLevel(secondLevelPath);

		return true;
	}
	if (m_CurrentLevel == 2)
	{
		std::string firstLevelPath{ "Levels/Level01.json" };
		m_Level02Completed = true;
		m_CurrentLevel = 1;
		LoadLevel(firstLevelPath);

		return true;
	}
	return false;
}

Level* LevelManager::GetLevel() const
{
	return m_pLevel;
}

Player* LevelManager::GetPlayer() const
{
	return m_pPlayer;
}

const std::vector<Enemy*>& LevelManager::GetEnemies() const
{
	return m_Enemies;
}

void LevelManager::RestarCurrentLevel()
{
	if (m_CurrentLevel == 1)
	{
		LoadLevel("Levels/Level01.json");
	}
	else if (m_CurrentLevel == 2)
	{
		LoadLevel("Levels/Level02.json");
	}
}

void LevelManager::ApplyRestart(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_r)
	{
		RestarCurrentLevel();

		return;
	}
}

void LevelManager::ClearLevel()
{
	delete m_pPlayer;
	m_pPlayer = nullptr;

	delete m_pLevel;
	m_pLevel = nullptr;

	for (Enemy* pEnemy : m_Enemies)
	{
		delete pEnemy;
	}
	m_Enemies.clear();
}

void LevelManager::SaveCurrentLevel()
{
	if (m_pPlayer != nullptr)
	{
		m_PlayerWeapon = m_pPlayer->GetWeaponType();
	}
	if (m_CurrentLevel == 1)
	{
		m_Level01State.completed = m_pLevel->AreAllEnemiesDead();
		m_Level01State.corpses = m_pLevel->GetCorpseData();
		m_Level01State.returnPosition = m_pLevel->GetLevelExitSignPosition();
	}
}

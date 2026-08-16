#pragma once
#include "JsonImporter.h"
#include "Corpse.h"
#include <string>

class Level;
class Player;
class Enemy;
class ResourceManager;
class Camera;
class VisualConfigManager;

class LevelManager
{
public:
	struct Level01State final
	{
		bool completed{};
		Vector2f returnPosition{};
		std::vector<Corpse::CorpseData> corpses{};
	};
	explicit LevelManager(ResourceManager* pResources, VisualConfigManager* pVisualConfig);

	~LevelManager();

	LevelManager(const LevelManager& other) = delete;
	LevelManager& operator=(const LevelManager& other) = delete;
	LevelManager(LevelManager&& other) = delete;
	LevelManager& operator=(LevelManager&& other) = delete;

	void LoadFirstLevel();
	bool TryChangeLevel();

	Level* GetLevel()const;
	Player* GetPlayer()const;

	const std::vector<Enemy*>& GetEnemies()const;

	void RestarCurrentLevel();
	void ApplyRestart(const SDL_KeyboardEvent& e);
private:
	void LoadLevel(const std::string& levelPath);
	void ClearLevel();

	void SaveCurrentLevel();

	Level* m_pLevel{};
	Player* m_pPlayer{};

	std::vector<Enemy*> m_Enemies{};

	ResourceManager* m_pResources{};
	VisualConfigManager* m_pVisualConfig{};

	JsonImporter::HitboxConfigData m_PlayerHitBoxConfig{};
	JsonImporter::HitboxConfigData m_EnemyHitBoxConfig{};

	Level01State m_Level01State{};

	WeaponType m_PlayerWeapon{  };

	int m_CurrentLevel{ 1 };
	bool m_Level02Completed{};
};


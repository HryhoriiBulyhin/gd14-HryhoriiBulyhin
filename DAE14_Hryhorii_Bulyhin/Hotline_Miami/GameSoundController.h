#pragma once

#include "WeaponType.h"
#include "Player.h"
#include "Enemy.h"
#include "Door.h"
#include "Car.h"

class SoundManager;
class Player;
class Enemy;
class Level;

class GameSoundController final
{
public:
	explicit GameSoundController();
	~GameSoundController();

	void Update(Player* pPlayer, const std::vector<Enemy*>& enemies, Level* pLevel);

private:
	void InitializeSounds();
	bool EnemiesChanged(const std::vector<Enemy*>& enemies)const;
	void SaveCurrentState(Player* pPlayer, const std::vector<Enemy*>& enemies);

	SoundManager* m_pSoundManager{};

	Player::PlayerState m_PreviousPlayerState{};
	WeaponType m_PreviousPlayerWeapon{};

	std::vector<Enemy*> m_PreviousEnemies{};
	std::vector<Enemy::EnemyState> m_PreviousEnemyStates{};
	std::vector<WeaponType> m_PreviousEnemyWeapons{};

	bool m_UseSecondSwing{};
	bool m_UseSecondCut{};

	int m_PreviousPlayerAnimationFrame{ };

#pragma region Doors
	bool DoorsChanged(const std::vector<Door*>& doors)const;
	void SaveDoorStates(const std::vector<Door*>& doors);
	std::vector<Door*> m_PreviousDoors{};
	std::vector<float>m_PreviousDoorAngles{};
	std::vector<bool> m_PreviousDoorPushedByPlayer{};
#pragma endregion Doors
	Car* m_pPreviousCar{};
	Car::CarState m_PreviousCarState{ Car::CarState::Closed };
};


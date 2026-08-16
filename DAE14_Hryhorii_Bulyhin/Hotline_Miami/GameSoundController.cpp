#include "pch.h"
#include "GameSoundController.h"

#include "SoundManager.h"
#include "Level.h"

GameSoundController::GameSoundController()
	:
	m_PreviousPlayerAnimationFrame{ -1 }
{
	m_pSoundManager = new SoundManager{};

	InitializeSounds();

	m_pSoundManager->PlayStream("music_prologue", true);
}

GameSoundController::~GameSoundController()
{
	delete m_pSoundManager;
	m_pSoundManager = nullptr;
}

void GameSoundController::InitializeSounds()
{
	m_pSoundManager->AddStream(
		"music_prologue",
		"Sounds/02. M.O.O.N - Paris.mp3"
	);
	m_pSoundManager->AddEffect(
		"cut1",
		"Sounds/sndCut1.wav"
	);

	m_pSoundManager->AddEffect(
		"cut2",
		"Sounds/sndCut2.wav"
	);

	m_pSoundManager->AddEffect(
		"door_hit",
		"Sounds/sndDoorHit.wav"
	);

	m_pSoundManager->AddEffect(
		"door_open",
		"Sounds/sndDoorOpen.wav"
	);

	m_pSoundManager->AddEffect(
		"hit",
		"Sounds/sndHit.wav"
	);

	m_pSoundManager->AddEffect(
		"pickup_weapon",
		"Sounds/sndPickupWeapon.wav"
	);

	m_pSoundManager->AddEffect(
		"punch",
		"Sounds/sndPunch.wav"
	);

	m_pSoundManager->AddEffect(
		"swing1",
		"Sounds/sndSwing1.wav"
	);

	m_pSoundManager->AddEffect(
		"swing2",
		"Sounds/sndSwing2.wav"
	);

	m_pSoundManager->AddEffect(
		"car_open",
		"Sounds/sndCarOpen.wav"
	);
	m_pSoundManager->AddEffect(
		"car_close",
		"Sounds/sndCarClose.wav"
	);
	m_pSoundManager->AddEffect(
		"car_engine",
		"Sounds/sndCarEngine.wav"
	);
	m_pSoundManager->AddEffect(
		"enemy_pickup",
		"Sounds/sndPickupWeapon.wav"
	);
}

void GameSoundController::Update(Player* pPlayer, const std::vector<Enemy*>& enemies, Level* pLevel)
{
	if (pPlayer == nullptr)
	{
		return;
	}

	const std::vector<Door*>& doors{ pLevel->GetDoors() };

	Car* pCar{ pLevel->GetCar() };

	if (pCar != nullptr)
	{
		const Car::CarState currentState{ pCar->GetState() };

		if (m_pPreviousCar != pCar)
		{
			m_pPreviousCar = pCar;
			m_PreviousCarState = currentState;
		}
		else
		{
			if (m_PreviousCarState == Car::CarState::Closed &&
				currentState == Car::CarState::Opening)
			{
				m_pSoundManager->PlayEffect("car_open");
			}
			if (m_PreviousCarState == Car::CarState::Open &&
				currentState == Car::CarState::Closing)
			{
				m_pSoundManager->PlayEffect("car_close");
			}
			if (m_PreviousCarState == Car::CarState::Open &&
				currentState == Car::CarState::Engine)
			{
				m_pSoundManager->PlayEffect("car_engine");
			}
			m_PreviousCarState = currentState;
		}
	}

	if (EnemiesChanged(enemies))
	{
		SaveCurrentState(
			pPlayer,
			enemies
		);
		SaveDoorStates(doors);

		return;
	}

	const Player::PlayerState currentPlayerState
	{
		pPlayer->GetState()
	};

	if (m_PreviousPlayerState != Player::PlayerState::Dead &&
		currentPlayerState == Player::PlayerState::Dead)
	{
		m_pSoundManager->PlayEffect("hit");
	}

	const WeaponType currentPlayerWeapon
	{
		pPlayer->GetWeaponType()
	};

	// Finisher
	const int currentPlayerFrame{ pPlayer->GetCurrentAnimationFrame() };

	if (currentPlayerState == Player::PlayerState::Finishing &&
		m_PreviousPlayerState == Player::PlayerState::Finishing &&
		currentPlayerFrame != m_PreviousPlayerAnimationFrame)
	{
		m_pSoundManager->PlayEffect("punch");
	}
	//Doors
	for (size_t i = 0; i < doors.size(); ++i)
	{
		Door* pDoor{ doors[i] };

		if (pDoor == nullptr)
		{
			continue;
		}
		const float previousAngle{ m_PreviousDoorAngles[i] };
		const float currentAngle{ pDoor->GetAngle() };
		const bool previousWasPushed{ m_PreviousDoorPushedByPlayer[i] };
		const bool currentWasPushed{ pDoor->WasPushedByPlayer() };

		if (!previousWasPushed && currentWasPushed)
		{
			m_pSoundManager->PlayEffect("door_open");
		}
	}
	SaveDoorStates(doors);
	// Player started an attack
	if (m_PreviousPlayerState != Player::PlayerState::Attacking &&
		currentPlayerState == Player::PlayerState::Attacking)
	{
		switch (currentPlayerWeapon)
		{
		case WeaponType::Bat:
		{
			if (m_UseSecondSwing)
			{
				m_pSoundManager->PlayEffect("swing2");
			}
			else
			{
				m_pSoundManager->PlayEffect("swing1");
			}

			m_UseSecondSwing = !m_UseSecondSwing;

			break;
		}

		case WeaponType::Knife:
		{
			if (m_UseSecondCut)
			{
				m_pSoundManager->PlayEffect("cut2");
			}
			else
			{
				m_pSoundManager->PlayEffect("cut1");
			}

			m_UseSecondCut = !m_UseSecondCut;

			break;
		}

		case WeaponType::Case:
		{
			if (m_UseSecondSwing)
			{
				m_pSoundManager->PlayEffect("swing2");
			}
			else
			{
				m_pSoundManager->PlayEffect("swing1");
			}
			break;
		}
		case WeaponType::Unarmed:
		{
			if (m_UseSecondSwing)
			{
				m_pSoundManager->PlayEffect("swing1");
			}
			else
			{
				m_pSoundManager->PlayEffect("swing2");
			}
			m_UseSecondSwing = !m_UseSecondSwing;
		}
		break;
		}
	}

	// Player picked up / changed weapon
	if (m_PreviousPlayerWeapon != currentPlayerWeapon)
	{
		m_pSoundManager->PlayEffect("pickup_weapon");
	}

	// Enemy state changes
	for (size_t i{}; i < enemies.size(); ++i)
	{
		Enemy* pEnemy{ enemies[i] };

		if (pEnemy == nullptr)
		{
			continue;
		}

		const Enemy::EnemyState previousState
		{
			m_PreviousEnemyStates[i]
		};

		const Enemy::EnemyState currentState
		{
			pEnemy->GetState()
		};
		const WeaponType previousWeapon
		{
			m_PreviousEnemyWeapons[i]
		};
		const WeaponType currentWeapon
		{
			pEnemy->GetWeaponType()
		};

		// Enemy picked up weapon
		if (previousWeapon == WeaponType::Unarmed &&
			currentWeapon != WeaponType::Unarmed)
		{
			m_pSoundManager->PlayEffect("enemy_pickup");
		}

		// Punch connected
		if (previousState != Enemy::EnemyState::Stunned &&
			currentState == Enemy::EnemyState::Stunned)
		{
			m_pSoundManager->PlayEffect("punch");
		}

		// Enemy died
		if (previousState != Enemy::EnemyState::Dead &&
			currentState == Enemy::EnemyState::Dead)
		{
			m_pSoundManager->PlayEffect("hit");
		}
	}

	SaveCurrentState(pPlayer, enemies);
	m_PreviousPlayerAnimationFrame = pPlayer->GetCurrentAnimationFrame();

}

bool GameSoundController::EnemiesChanged(const std::vector<Enemy*>& enemies) const
{
	if (m_PreviousEnemies.size() != enemies.size())
	{
		return true;
	}

	for (size_t i = 0; i < enemies.size(); ++i)
	{
		if (m_PreviousEnemies[i] != enemies[i])
		{
			return true;
		}
	}

	return false;
}

void GameSoundController::SaveCurrentState(Player* pPlayer, const std::vector<Enemy*>& enemies)
{
	m_PreviousPlayerState = pPlayer->GetState();
	m_PreviousPlayerWeapon = pPlayer->GetWeaponType();
	m_PreviousPlayerAnimationFrame = pPlayer->GetCurrentAnimationFrame();

	m_PreviousEnemies = enemies;

	m_PreviousEnemyStates.resize(enemies.size());
	m_PreviousEnemyWeapons.resize(enemies.size());

	for (size_t i = 0; i < enemies.size(); ++i)
	{
		if (enemies[i] == nullptr)
		{
			continue;
		}

		m_PreviousEnemyStates[i] =
			enemies[i]->GetState();
		m_PreviousEnemyWeapons[i] =
			enemies[i]->GetWeaponType();
	}
}

bool GameSoundController::DoorsChanged(const std::vector<Door*>& doors) const
{
	if (m_PreviousDoors.size() != doors.size())
	{
		return true;
	}
	for (size_t i = 0; i < doors.size(); ++i)
	{
		if (m_PreviousDoors[i] != doors[i])
		{
			return true;
		}
	}
	return false;
}

void GameSoundController::SaveDoorStates(const std::vector<Door*>& doors)
{
	m_PreviousDoors = doors;

	m_PreviousDoorAngles.resize(doors.size());
	m_PreviousDoorPushedByPlayer.resize(doors.size());

	for (size_t i = 0; i < doors.size(); ++i)
	{
		if (doors[i] == nullptr)
		{
			continue;
		}

		m_PreviousDoorAngles[i] = doors[i]->GetAngle();
		m_PreviousDoorPushedByPlayer[i] = doors[i]->WasPushedByPlayer();
	}
}

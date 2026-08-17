#include "pch.h"
#include "Level.h"
#include "Door.h"
#include "Texture.h"
#include "LevelPositioner.h"
#include "Entity.h"
#include "utils.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "Player.h"
#include "EnemyNavigator.h"
#include "Sign.h"
#include "Car.h"

Level::Level(const std::string& jsonPath)
{
	Load(jsonPath);
}

Level::~Level()
{
	Clear();
}
void Level::Clear()
{
	delete m_pBackground;
	m_pBackground = nullptr;

	for (DroppedItem* pDroppedItem : m_DroppedItems)
	{
		delete pDroppedItem;
		pDroppedItem = nullptr;
	}
	m_DroppedItems.clear();
	for (Door* pDoors : m_Doors)
	{
		delete pDoors;
	}
	m_Doors.clear();

	delete m_pEnemyNavigator;
	m_pEnemyNavigator = nullptr;

	delete m_pLevelExitSign;
	m_pLevelExitSign = nullptr;

	for (Corpse* pCorpse : m_Corpses)
	{
		delete pCorpse;
	}
	m_Corpses.clear();

	delete m_pCar;
	m_pCar = nullptr;
}

void Level::Load(const std::string& jsonPath)
{
	m_Data = JsonImporter::ImportGameInfo(jsonPath);

	const std::string backgroundPath
	{
		"Backgrounds/" +
		m_Data.backgroundTexturePath
	};

	m_pBackground = new Texture{ backgroundPath };

	if (m_Data.coordinateMode == JsonImporter::CoordinateMode::LegacyUnityUnits)
	{
		m_LevelScale = 1.2f;

		PositionLevelData();
	}
	else
	{
		m_LevelScale = 1.f;
	}
	ScaleLevelData();
}

void Level::LoadDoors()
{
	for (const JsonImporter::DoorInfo& doorInfo : m_Data.doors)
	{
		Door* pDoor{ new Door{doorInfo,m_pResources} };
		m_Doors.push_back(pDoor);
	}
}

void Level::LoadCar()
{
	if (!m_Data.hasCar)
	{
		return;
	}
	m_pCar = new Car
	{
		m_Data.carPosition,
		m_pResources
	};
}


void Level::PositionLevelData()
{
	const Vector2f offset
	{
		m_pBackground->GetWidth() / 2.f,
		m_pBackground->GetHeight() / 2.f
	};

	LevelPositioner::ApplyOffset(
		m_Data,
		offset);
}

void Level::ScaleLevelData()
{
	for (Rectf& collision : m_Data.collisions)
	{
		collision.left *= m_LevelScale;
		collision.bottom *= m_LevelScale;
		collision.width *= m_LevelScale;
		collision.height *= m_LevelScale;
	}
	if (m_Data.hasPlayerSpawn)
	{
		m_Data.playerSpawn.x *= m_LevelScale;
		m_Data.playerSpawn.y *= m_LevelScale;
	}
	if (m_Data.hasLevelExit)
	{
		m_Data.levelExit.position.x *= m_LevelScale;
		m_Data.levelExit.position.y *= m_LevelScale;

		m_Data.levelExit.collider.left *= m_LevelScale;
		m_Data.levelExit.collider.bottom *= m_LevelScale;
		m_Data.levelExit.collider.width *= m_LevelScale;
		m_Data.levelExit.collider.height *= m_LevelScale;
	}
	for (JsonImporter::EnemyInfo& enemy : m_Data.enemies)
	{
		enemy.position.x *= m_LevelScale;
		enemy.position.y *= m_LevelScale;
	}
	for (JsonImporter::DoorInfo& door : m_Data.doors)
	{
		door.position.x *= m_LevelScale;
		door.position.y *= m_LevelScale;

		door.rect.left *= m_LevelScale;
		door.rect.bottom *= m_LevelScale;
		door.rect.width *= m_LevelScale;
		door.rect.height *= m_LevelScale;
	}
	for (JsonImporter::SignInfo& sign : m_Data.signs)
	{
		sign.position.x *= m_LevelScale;
		sign.position.y *= m_LevelScale;
	}
	for (JsonImporter::WayPointInfo& wayPoint : m_Data.wayPoints)
	{
		wayPoint.position.x *= m_LevelScale;
		wayPoint.position.y *= m_LevelScale;
	}
	if (m_Data.hasCar)
	{
		m_Data.carPosition.x *= m_LevelScale;
		m_Data.carPosition.y *= m_LevelScale;
	}
}

bool Level::CanEnemySeePlayer(const Enemy& enemy) const
{
	return
		m_CollisionManager.HasLineOfSight(
			enemy.GetCenter(), m_pPlayer->GetCenter(), m_Data.collisions, m_Doors
		);
}

void Level::UpdateLevelExitSign()
{
	if (!AreAllEnemiesDead())
	{
		return;
	}

	if (m_pLevelExitSign != nullptr)
	{
		return;
	}

	for (const JsonImporter::SignInfo& signInfo : m_Data.signs)
	{
		if (signInfo.subType == "LevelExit")
		{
			m_pLevelExitSign = new Sign{ signInfo,m_pResources };

			return;
		}
	}
}

std::vector<Vector2f> Level::GetPatrolPoints(int enemyIndex) const
{
	std::vector<JsonImporter::PatrolPointInfo> patrolPoints{};

	const std::string patrolId{ "EnemyPatrol_" + std::to_string(enemyIndex) };

	for (const JsonImporter::PatrolPointInfo& patrolPoint : m_Data.patrolPoints)
	{
		if (patrolPoint.patrolId == patrolId)
		{
			patrolPoints.push_back(patrolPoint);
		}
	}
	std::sort(patrolPoints.begin(), patrolPoints.end(),
		[](const JsonImporter::PatrolPointInfo& first, const JsonImporter::PatrolPointInfo& second)
		{
			return first.index < second.index;
		}
	);
	std::vector<Vector2f> positions{};

	for (const JsonImporter::PatrolPointInfo& patrolPoint : patrolPoints)
	{
		positions.push_back(patrolPoint.position);
	}

	return positions;
}

void Level::Draw() const
{
	const Rectf dstRect
	{
		0.f,
		0.f,
		m_pBackground->GetWidth() * m_LevelScale,
		m_pBackground->GetHeight() * m_LevelScale
	};

	m_pBackground->Draw(dstRect);


	/*utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });

	for (const Rectf& collision : m_Data.collisions)
	{
		utils::DrawRect(collision, 2.f);
	}*/

	for (Door* pDoor : m_Doors)
	{
		pDoor->Draw();
	}

	if (m_pLevelExitSign != nullptr)
	{
		m_pLevelExitSign->Draw();
	}

	//// Debug WayPoints
	//for (const JsonImporter::WayPointInfo& wayPoint : m_Data.wayPoints)
	//{
	//	utils::SetColor(Color4f{ 1.f, 0.f, 1.f, 1.f });

	//	utils::FillEllipse(
	//		wayPoint.position,
	//		8.f,
	//		8.f
	//	);
	//}

	for (Corpse* pCorpse : m_Corpses)
	{
		pCorpse->Draw();
	}
}

void Level::DrawDroppedItems() const
{
	for (DroppedItem* pDroppedItem : m_DroppedItems)
	{
		pDroppedItem->Draw();
	}
}

void Level::DrawCar() const
{
	if (m_pCar != nullptr)
	{
		m_pCar->Draw();
	}
}

void Level::DrawDoors() const
{
	for (Door* pDoor : m_Doors)
	{
		pDoor->Draw();
	}
}


void Level::Update(float elapsedSec)
{
	UpdateEnemyWeaponDrops();
	UpdateEnemyWeaponSearch();
	UpdateEnemyWeaponPickUp();

	for (Enemy* pEnemy : m_Enemies)
	{
		pEnemy->SetCanSeePlayer(CanEnemySeePlayer(*pEnemy));

		m_pEnemyNavigator->Update(
			pEnemy,
			m_pPlayer,
			elapsedSec
		);
	}
	for (DroppedItem* pDroppedItem : m_DroppedItems)
	{
		pDroppedItem->Update(elapsedSec);
	}

	for (Door* pDoor : m_Doors)
	{
		pDoor->HandlePlayer(m_pPlayer, m_pPlayer->GetVelocity());

		for (Enemy* pEnemy : m_Enemies)
		{
			pDoor->HandleEnemy(pEnemy, pEnemy->GetVelocity());
		}
		pDoor->Update(elapsedSec);
	}
	if (m_pCar != nullptr)
	{
		m_pCar->HandlePlayer(m_pPlayer);
		m_pCar->Update(elapsedSec);
	}
	if (m_pPlayer->WantsToPickUp())
	{
		TryPickUpDroppedItem();
		m_pPlayer->ResetPickUpRequest();
	}

	if (m_pPlayer->WantsToFinisher())
	{
		TryStartFinisher();
		m_pPlayer->ResetFinisherRequest();
	}
	UpdateLevelExitSign();

	if (m_pLevelExitSign != nullptr)
	{
		m_pLevelExitSign->Update(elapsedSec);
	}

	RemovePickedUpItems();
}

const JsonImporter::GameData& Level::GetData() const
{
	return m_Data;
}

JsonImporter::GameData& Level::GetData()
{
	return m_Data;
}

void Level::LoadEntities(std::vector<Enemy*>& enemies, const JsonImporter::EntityHitboxInfo& hitboxInfo, const JsonImporter::EnemyVisualInfo& visualInfo, ResourceManager* pResources, bool loadEnemies)
{
	m_pResources = pResources;

	if (loadEnemies)
	{
		const std::vector<JsonImporter::EnemyInfo>& enemiesInfos{ m_Data.enemies };

		for (const JsonImporter::EnemyInfo& enemiesInfo : enemiesInfos)
		{
			Enemy* pEnemy{ new Enemy{enemiesInfo.position,hitboxInfo,visualInfo,pResources} };

			if (enemiesInfo.index > 0)
			{
				const std::vector<Vector2f> patrolPoints{ GetPatrolPoints(enemiesInfo.index) };
				pEnemy->SetPatrolPoints(patrolPoints);
				if (!patrolPoints.empty())
				{
					pEnemy->SetState(Enemy::EnemyState::Patrolling);
				}
			}

			if (enemiesInfo.subType == "Bat" || enemiesInfo.subType == "Door")
			{
				pEnemy->SetWeaponType(WeaponType::Bat);
			}
			else if (enemiesInfo.subType == "Knife" || enemiesInfo.subType == "Toilet")
			{
				pEnemy->SetWeaponType(WeaponType::Knife);
			}
			else if (enemiesInfo.subType == "Case")
			{
				pEnemy->SetWeaponType(WeaponType::Case);
			}
			else
			{
				pEnemy->SetWeaponType(WeaponType::Unarmed);
			}
			enemies.push_back(pEnemy);
			m_Enemies.push_back(pEnemy);
		}
	}
	// Loading doors
	LoadDoors();

	// Load Car
	LoadCar();
	//Created enemyNavigator
	m_pEnemyNavigator = new EnemyNavigator{ m_Data.wayPoints,m_CollisionManager,m_Data.collisions,m_Doors };
}

bool Level::IsEntityColliding(const Entity& entity) const
{
	if (m_CollisionManager.IsEntityCollidingWithLevel(entity, m_Data.collisions))
	{
		return true;
	}
	if (m_pCar != nullptr && &entity == m_pPlayer && m_pCar->IsCollidingWithPlayer(*m_pPlayer))
	{
		return true;
	}
	return false;
}

bool Level::ResolveEntityMovement(Entity& entity, const Vector2f& previousPos) const
{
	bool collided{ false };

	const Vector2f desiredPos{ entity.GetPosition() };

	entity.SetPosition(Vector2f
		{
			desiredPos.x,
			previousPos.y
		}
	);
	entity.RefreshHitbox();

	if (IsEntityColliding(entity))
	{
		collided = true;
		entity.SetPosition(Vector2f
			{
				previousPos.x,
				previousPos.y
			}
		);
		entity.RefreshHitbox();
	}
	const float resolvedX{ entity.GetPosition().x };

	// Try vertical movement

	entity.SetPosition(Vector2f
		{
			resolvedX,
			desiredPos.y
		}
	);

	entity.RefreshHitbox();

	if (IsEntityColliding(entity))
	{
		collided = true;

		entity.SetPosition(Vector2f
			{
				resolvedX,
				previousPos.y
			}
		);
		entity.RefreshHitbox();
	}
	if (collided)
	{
		entity.OnLevelCollision();
	}
	return collided;
}

void Level::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

const std::vector<Door*>& Level::GetDoors() const
{
	return m_Doors;
}

Car* Level::GetCar() const
{
	return m_pCar;
}

void Level::UpdateEnemyWeaponDrops()
{
	for (Enemy* pEnemy : m_Enemies)
	{
		const bool isStunned{ pEnemy->GetState() == Enemy::EnemyState::Stunned };
		const bool isDead{ pEnemy->GetState() == Enemy::EnemyState::Dead };

		if (!isStunned && !isDead)
		{
			continue;
		}
		if (pEnemy->GetWeaponType() == WeaponType::Unarmed)
		{
			continue;
		}
		if (pEnemy->HasDroppedItem())
		{
			continue;
		}

		const WeaponType droppedWeapon{ pEnemy->GetWeaponType() };
		DroppedItem* pDroppedItem
		{
			new DroppedItem
			{
				pEnemy,
				m_pPlayer,
				droppedWeapon,
				m_pResources
			}
		};
		m_DroppedItems.push_back(pDroppedItem);
		pEnemy->SetHasDroppedWeapon(true);
		if (isStunned)
		{
			pEnemy->SetWeaponType(WeaponType::Unarmed);
		}
	}
}

void Level::UpdateEnemyWeaponSearch()
{
	for (Enemy* pEnemy : m_Enemies)
	{
		if (pEnemy->GetWeaponType() != WeaponType::Unarmed)
		{
			continue;
		}
		const Enemy::EnemyState state{ pEnemy->GetState() };

		if (state != Enemy::EnemyState::Fleeing &&
			state != Enemy::EnemyState::Patrolling)
		{
			continue;
		}

		for (DroppedItem* pItem : m_DroppedItems)
		{
			if (pItem == nullptr || pItem->IsPickedUp())
			{
				continue;
			}
			pEnemy->SetSearchTarget(pItem->GetPosition());
			pEnemy->SetState(Enemy::EnemyState::SearchingWeapon);
			break;
		}
	}
}

void Level::UpdateEnemyWeaponPickUp()
{
	for (Enemy* pEnemy : m_Enemies)
	{
		if (pEnemy->GetState() != Enemy::EnemyState::SearchingWeapon)
		{
			continue;
		}
		for (DroppedItem* pItem : m_DroppedItems)
		{
			if (pItem == nullptr || pItem->IsPickedUp())
			{
				continue;
			}
			pItem->TryPickUpByEnemy(pEnemy);
			if (pItem->IsPickedUp())
			{
				break;
			}
		}
	}
}


void Level::RemovePickedUpItems()
{
	std::vector<DroppedItem*>::iterator iterator{ m_DroppedItems.begin() };

	while (iterator != m_DroppedItems.end())
	{
		DroppedItem* pDroppedItem{ *iterator };

		if (!pDroppedItem->IsPickedUp())
		{
			++iterator;
			continue;
		}
		delete pDroppedItem;
		iterator = m_DroppedItems.erase(iterator);
	}
}

void Level::TryPickUpDroppedItem()
{
	for (DroppedItem* pDroppedItem : m_DroppedItems)
	{
		pDroppedItem->TryPickUpByPlayer();

		if (pDroppedItem->IsPickedUp())
		{
			return;
		}
	}
}

void Level::TryStartFinisher()
{
	for (Enemy* pEnemy : m_Enemies)
	{
		if (m_pPlayer->CanStartFinisher(pEnemy))
		{
			m_pPlayer->StartFinisher(pEnemy);
			return;
		}
	}
}

bool Level::AreAllEnemiesDead() const
{
	if (m_Enemies.empty())
	{
		return false;
	}

	for (Enemy* pEnemy : m_Enemies)
	{
		if (pEnemy->GetState() != Enemy::EnemyState::Dead)
		{
			return false;
		}
	}
	return true;
}

bool Level::IsPlayerOnLevelExit() const
{
	if (!m_Data.hasLevelExit)
	{
		return false;
	}
	if (!AreAllEnemiesDead())
	{
		return false;
	}
	return utils::IsOverlapping(
		m_pPlayer->GetBodyHitBox().GetCurrentPoints(),
		m_Data.levelExit.collider
	);
}

Vector2f Level::GetLevelExitSignPosition() const
{
	// Getting sign position for the player to return on level 01

	for (const JsonImporter::SignInfo& sign : m_Data.signs)
	{
		if (sign.subType == "LevelExit")
		{
			return sign.position;
		}
	}
	return {};
}

std::vector<Corpse::CorpseData> Level::GetCorpseData() const
{
	std::vector<Corpse::Corpse::CorpseData> corpses{};

	for (Enemy* pEnemy : m_Enemies)
	{
		if (pEnemy->GetState() != Enemy::EnemyState::Dead)
		{
			continue;
		}
		const Corpse::CorpseData corpse
		{
			pEnemy->GetCenter(),
			pEnemy->GetRotationAngle(),
			pEnemy->GetWeaponType()
		};
		corpses.push_back(corpse);
	}
	return corpses;
}

void Level::LoadCorpses(const std::vector<Corpse::CorpseData>& corpseData)
{
	for (const Corpse::CorpseData& data : corpseData)
	{
		Corpse* pCorpse
		{
			new Corpse
			{
				data,
				m_pResources
			}
		};
		m_Corpses.push_back(pCorpse);
	}
}

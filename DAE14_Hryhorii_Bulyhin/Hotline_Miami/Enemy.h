#pragma once
#include "Entity.h"
#include "JsonImporter.h"
#include "WeaponType.h"
#include "Animation.h"
#include "EnemyPatrol.h"

class ResourceManager;
class Texture;
class Player;

#pragma region EnemyData
struct EnemyAnimationData
{
	Texture* pTexture{};

	int rows{};
	int columns{};
	int frameCount{};

	float frameTime{};
	float scale{};

	bool loop{};

	std::vector<JsonImporter::HitboxFrameInfo> hitBoxFrames{};
};
struct EnemyPresetData
{
	EnemyAnimationData walk{};
	EnemyAnimationData attack{};
	EnemyAnimationData dead{};

	bool hasDead{};
};
#pragma endregion EnemyData

class Enemy final : public Entity
{
public:

#pragma region EnemyState
	enum class EnemyState
	{
		Walking,
		Attacking,
		Stunned,
		Fleeing,
		Patrolling,
		BeingFinished,
		SearchingWeapon,
		Dead
	};
#pragma endregion EnemyState

#pragma region Entity
	explicit Enemy(const Vector2f& position, const JsonImporter::EntityHitboxInfo& hitboxInfo, const JsonImporter::EnemyVisualInfo& visualInfo, ResourceManager* pResources);
	void Update(float elapsedSec)override;
	void Draw() const override;
	void RefreshHitbox()override;
	void Hit() override;
	void OnLevelCollision()override;
#pragma endregion Entity

	Enemy(const Enemy& other) = delete;
	Enemy& operator=(const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

#pragma region State
	void SetState(EnemyState state);
	EnemyState GetState()const;
	void Stun(const Vector2f& hitDir);
#pragma endregion State

	void SetWeaponType(WeaponType weaponType);
	void SetTargetPlayer(Player* pPlayer);

	WeaponType GetWeaponType()const;
	Vector2f GetCenter()const;

	void SetSearchTarget(const Vector2f& targetPosition);
	float GetRotationAngle()const;

	void SetCanSeePlayer(bool canSeePlayer);
	bool CanSeePlayer()const;

	void ReversePatrolDirection();
	void MoveTowards(const Vector2f& target, float elapsedSec);

	void SetNavigationTarget(const Vector2f& target);
	void ClearNavigationTarget();
	bool HasNavigationTarget()const;

	void SetPatrolPoints(const std::vector<Vector2f>& points);

	bool HasDroppedItem()const;
	void SetHasDroppedWeapon(bool hasDroppedWeapon);
private:

#pragma region StateUpdates
	void UpdateCurrentState(float elapsedSec);

	void UpdateWalking(float elapsedSec);
	void UpdateAttacking(float elapsedSec);
	void UpdateStunned(float elapsedSec);
	void UpdatePatrolling(float elapsedSec);
	void UpdateFleeing(float elapsedSec);
	void UpdateSearchingWeapon(float elapsedSec);
	void UpdateBeingFinished(float elapsedSec);
	void UpdateDead(float elapsedSec);
#pragma endregion StateUpdates

	void MoveDirection(const Vector2f& direction, float speed, float elapsedSec);
	void RotateTowards(const Vector2f& target);

	void UpdateCombat();

	void UpdateBodyHitBox();
	void UpdateCombatHitBox();

#pragma region Animation
	void LoadAnimationData(EnemyAnimationData& dst, const JsonImporter::AnimationVisualInfo& src, const JsonImporter::AnimationHitboxInfo* pHitboxInfo, ResourceManager* pResources);
	const JsonImporter::HitboxPresetInfo* FindHitboxPreset(const JsonImporter::EntityHitboxInfo& hitBoxInfo, const std::string& id)const;

	const EnemyAnimationData* GetCurrentAnimationData() const;
	void ApplyCurrentAnimation();
#pragma endregion Animation

	EnemyPresetData m_Presets[static_cast<int>(WeaponType::Count)];
	EnemyAnimationData m_Dead{};

	Texture* m_pCurrentTexture{};

	Animation m_CurrentAnimation{};
	WeaponType m_WeaponType{};
	EnemyState m_State{};
	Player* m_TargertPlayer{};

	float m_Scale{};
	float m_RotationAngle{};
	float m_MoveSpeed{};

	float m_StunnedTime{},
		m_GetUpDealay{};

	bool m_IsMoving{};

	Vector2f m_SearchTarget{};
	float m_FleeDistance{},
		m_FleeSpeed{};

	bool m_CanSeePlayer{};
	bool m_HasDroppedItem{};

	float m_PatrolDircetion{},
		m_PatrolSpeed{};

	Vector2f m_NavigationTarget{};
	bool m_HasNavigationTarget{};

	EnemyPatrol m_Patrol{};
};

#pragma once
#include "Entity.h"
#include "JsonImporter.h"
#include "WeaponType.h"
#include "Animation.h"

class HitBox;
class ResourceManager;
class Texture;
class Enemy;

#pragma region PlayerData
struct PlayerAnimationData final
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
struct PlayerPresetData final
{
	PlayerAnimationData walk{};
	PlayerAnimationData attack{};
};
#pragma endregion PlayerData

class Player final : public Entity
{
public:
	enum class PlayerState
	{
		Walking,
		Attacking,
		Dead,
		Finishing
	};
	explicit Player(const Vector2f& position, const JsonImporter::EntityHitboxInfo& hitboxInfo, const JsonImporter::PlayerVisualInfo& visualInfo, ResourceManager* pResources);

	Player(const Player& other) = delete;
	Player& operator=(const Player& other) = delete;
	Player(Player&& other) = delete;
	Player& operator=(Player&& other) = delete;

#pragma region Entity
	void Update(float elapsedSec) override;
	void Draw()const override;
	void Hit() override;
	void RefreshHitbox()override;
#pragma endregion Entity

	void UpdateCombatHitBox();

	void SetWeaponType(WeaponType weaponType);
	void SetState(PlayerState state);

	WeaponType GetWeaponType()const;
	PlayerState GetState() const;

#pragma region Movement
	void UpdateMovement(float elapsedSec);
	Vector2f GetCenter()const;
	Vector2f GetBodyCenter()const;
	void SetTargetCrosshair(const Vector2f& targetPos);
#pragma endregion Movement

#pragma region Input
	void MouseHandling(const SDL_MouseButtonEvent& e);
	void KeyboardHandling(const SDL_KeyboardEvent& e);
#pragma endregion Input

	void StartAttack();
	void AdvanceFinisher();
	void StartFinisher(Enemy* pEnemy);

	bool CanStartFinisher(Enemy* pEnemy)const;

#pragma region Requests
	bool WantsToPickUp()const;
	void ResetPickUpRequest();
	bool WantsToAttack()const;
	void ResetAttackRequest();
	bool WantsToFinisher()const;
	void ResetFinisherRequest();
#pragma endregion Requests
	int GetCurrentAnimationFrame()const;
	void SetTargetEnemy(Enemy* pEnemy);
private:

#pragma region StateUpdates
	void UpdateCurrentState(float elapsedSec);

	void UpdateWalking(float elapsedSec);
	void UpdateAttacking(float elapsedSec);
	void UpdateDead(float elapsedSec);
	void UpdateFinishing(float elapsedSec);
#pragma endregion StateUpdates

#pragma region Animation
	void UpdateBodyHitbox();
	const JsonImporter::HitboxPresetInfo* FindHitboxPreset(const JsonImporter::EntityHitboxInfo& hitboxInfo, const std::string& id)const;
	void LoadAnimationData(PlayerAnimationData& destination, const JsonImporter::AnimationVisualInfo& source, const JsonImporter::AnimationHitboxInfo* pHitboxInfo, ResourceManager* pResources);
	const PlayerAnimationData* GetCurrentAnimationData()const;
	void ApplyCurrentAnimation();
#pragma endregion Animation

	PlayerPresetData m_Presets[static_cast<int>(WeaponType::Count)];
	PlayerAnimationData m_Dead{};
	PlayerAnimationData m_Finisher{};

	Texture* m_pCurrentTexture{};

	Animation m_CurrentAnimation{};

	WeaponType m_WeaponType{};
	PlayerState m_State{};
	Enemy* m_pFinisherTarget{};
	Enemy* m_pTargetEnemy{};

	float m_Scale{};
	bool m_WasMoving{};
	float m_RotationAngle{};
	bool m_WantsToPickUp{},
		m_WantsToAttack{};
	bool m_WantsToFinisher{};
};

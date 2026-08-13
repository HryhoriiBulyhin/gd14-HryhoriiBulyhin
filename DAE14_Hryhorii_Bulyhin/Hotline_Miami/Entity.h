#pragma once

#include "HitBox.h"
class Entity
{
public:
	enum class EntityType
	{
		player,
		enemy
	};
	explicit Entity(EntityType type, const Vector2f& position);

	virtual ~Entity() = default;

	virtual void Update(float elapsedSec) = 0;
	virtual void Draw()const = 0;
	virtual void RefreshHitbox() = 0;
	virtual void Hit() = 0;
	virtual void OnLevelCollision();

	void SetPosition(const Vector2f& position);
	void SetVelocity(const Vector2f& velocity);

	void SetActive(bool isActive);
	void SetAlive(bool isAlive);

	const Vector2f& GetPosition() const;
	const Vector2f& GetVelocity() const;

#pragma region FallDirectionXFriction
	void SetFallDirection(const Vector2f& hitDirection);
	const Vector2f& GetFallDirection() const;
	float GetFallAngle() const;
	void UpdateFriction(float elapsedSec);
	float GetFallForce()const;
#pragma endregion FallDirectionXFriction

#pragma region Hitbox
	HitBox& GetBodyHitBox();
	const HitBox& GetBodyHitBox() const;

	HitBox& GetCombatHitBox();
	const HitBox& GetCombatHitBox() const;
	const std::vector<Vector2f>& GetCurrentHitBox() const;
#pragma endregion Hitbox

	void SavePreviousPosition();
	const Vector2f& GetPreviousPos()const;

	EntityType GetType()const;

	bool IsActive() const;
	bool IsAlive()const;
private:
	EntityType m_Type{};

	Vector2f m_Position{};
	Vector2f m_Velocity{};

	HitBox m_BodyHitBox{},
		m_CombatHitBox{};

	bool m_IsActive{};
	bool m_IsAlive{};

	Vector2f m_PreviousPos{};

	float m_FallAngle{};
	Vector2f m_FallDirection{};
	float m_Friction{},
		m_FallForce{};
};


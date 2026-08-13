#include "pch.h"
#include "Entity.h"
#include "WorldObject.h"
#include "CollisionManager.h"


Entity::Entity(EntityType type, const Vector2f& position)
	:
	m_Type{ type },
	m_Position{ position },
	m_PreviousPos{ position },
	m_IsActive{ true },
	m_IsAlive{ true },
	m_Friction{ 800.f },
	m_FallForce{ 300.f }
{
}

void Entity::SetPosition(const Vector2f& position)
{
	m_Position = position;
}

void Entity::SetVelocity(const Vector2f& velocity)
{
	m_Velocity = velocity;
}

void Entity::SetActive(bool isActive)
{
	m_IsActive = isActive;
}

void Entity::SetAlive(bool isAlive)
{
	m_IsAlive = isAlive;
}

const Vector2f& Entity::GetVelocity() const
{
	return m_Velocity;
}

void Entity::SetFallDirection(const Vector2f& hitDirection)
{
	Vector2f direction{ hitDirection };

	if (direction.Length() > 0.f)
	{
		direction = direction.Normalized();
	}

	m_FallDirection = direction;

	m_FallAngle = std::atan2(direction.y, direction.x) * 180.f / static_cast<float>(M_PI);
}

const Vector2f& Entity::GetFallDirection() const
{
	return m_FallDirection;
}

float Entity::GetFallAngle() const
{
	return m_FallAngle;
}

void Entity::UpdateFriction(float elapsedSec)
{
	if (GetVelocity().Length() <= 0.f)
	{
		return;
	}
	const float speed{ GetVelocity().Length() };

	float newSpeed{ speed - m_Friction * elapsedSec };

	if (newSpeed < 0.f)
	{
		newSpeed = 0.f;
	}
	if (speed > 0.f)
	{
		SetVelocity(GetVelocity().Normalized() * newSpeed);
	}
	SetPosition(GetPosition() + GetVelocity() * elapsedSec);
}

float Entity::GetFallForce() const
{
	return m_FallForce;
}

HitBox& Entity::GetBodyHitBox()
{
	return m_BodyHitBox;
}
const HitBox& Entity::GetBodyHitBox() const
{
	return m_BodyHitBox;
}

HitBox& Entity::GetCombatHitBox()
{
	return m_CombatHitBox;
}

const HitBox& Entity::GetCombatHitBox() const
{
	return m_CombatHitBox;
}
void Entity::SavePreviousPosition()
{
	m_PreviousPos = m_Position;
}

const Vector2f& Entity::GetPreviousPos() const
{
	return m_PreviousPos;
}
const std::vector<Vector2f>& Entity::GetCurrentHitBox() const
{
	return m_BodyHitBox.GetCurrentPoints();
}

Entity::EntityType Entity::GetType() const
{
	return m_Type;
}

void Entity::OnLevelCollision()
{

}

const Vector2f& Entity::GetPosition() const
{
	return m_Position;
}

bool Entity::IsActive() const
{
	return m_IsActive;
}

bool Entity::IsAlive() const
{
	return m_IsAlive;
}

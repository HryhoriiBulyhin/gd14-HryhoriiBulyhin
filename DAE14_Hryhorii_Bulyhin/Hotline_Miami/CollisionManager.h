#pragma once
#include <vector>

class Entity;
class Door;
class CollisionManager final
{
public:
	CollisionManager() = default;

	bool IsEntityCollidingWithLevel(const Entity& entity, const std::vector<Rectf>& levelCollisions)const;
	const Rectf* FindEntityLevelCollision(const Entity& entity, const std::vector<Rectf>& levelCollisions)const;
	const std::vector<Vector2f>& GetCurrentHitBox() const;
	bool HasLineOfSight(const Vector2f& from, const Vector2f& to, const std::vector<Rectf>& collisions, const std::vector<Door*>& doors)const;
};
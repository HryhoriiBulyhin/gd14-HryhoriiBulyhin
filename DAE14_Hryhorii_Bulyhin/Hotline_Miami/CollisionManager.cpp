#include "pch.h"
#include "CollisionManager.h"
#include "Entity.h"
#include "utils.h"
#include "Door.h"

bool CollisionManager::IsEntityCollidingWithLevel(const Entity& entity, const std::vector<Rectf>& levelCollisions) const
{
    return FindEntityLevelCollision(entity, levelCollisions);
}

const Rectf* CollisionManager::FindEntityLevelCollision(const Entity& entity, const std::vector<Rectf>& levelCollisions) const
{
    const std::vector<Vector2f>& entityHitBox{ entity.GetCurrentHitBox() };

    for (const Rectf& levelCollision : levelCollisions)
    {
        if (utils::IsOverlapping(entityHitBox, levelCollision))
        {
            return &levelCollision;
        }
    }
    return nullptr;
}
bool CollisionManager::HasLineOfSight(const Vector2f& from, const Vector2f& to, const std::vector<Rectf>& collisions, const std::vector<Door*>& doors) const
{
    for (const Rectf& collision : collisions)
    {
        if (utils::IsOverlapping(from, to, collision))
        {
            return false;
        }
    }
    for (const Door* pDoor : doors)
    {
        if (pDoor == nullptr)
        {
            continue;
        }
        if (!pDoor->IsSolid())
        {
            continue;
        }
        utils::HitInfo hitInfo{};

        if (utils::Raycast(pDoor->GetCurrentPoints(), pDoor->GetVertexCount(), from, to, hitInfo))
        {
            return false;
        }
    }
    return true;
}

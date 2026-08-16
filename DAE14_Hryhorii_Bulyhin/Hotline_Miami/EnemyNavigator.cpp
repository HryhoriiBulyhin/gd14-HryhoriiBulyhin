#include "pch.h"
#include "EnemyNavigator.h"
#include "Enemy.h"
#include "Player.h"

#include <iostream>
EnemyNavigator::EnemyNavigator(const std::vector<JsonImporter::WayPointInfo>& wayPoints, const CollisionManager& collisionManager, const std::vector<Rectf>& collisions, const std::vector<Door*>& doors)
	:
	m_PathFinder{ wayPoints },
	m_WayPoints{ wayPoints },
	m_CollisionManager{ collisionManager },
	m_Collisions{ collisions },
	m_Doors{ doors },
	m_ReachDistance{ 15.f }
{
}

void EnemyNavigator::Update(Enemy* pEnemy, Player* pPlayer, float elapsedSec)
{
	NavigationData* pData{ FindNavigationData(pEnemy) };

	if (pData == nullptr)
	{
		m_NavigationData.push_back(NavigationData{ pEnemy,{},0,false });
		pData = &m_NavigationData.back();
	}
	if (pEnemy->GetState() == Enemy::EnemyState::Stunned)
	{
		ResetNavigation(*pData, pEnemy);
		return;
	}
	if (pEnemy->CanSeePlayer())
	{
		pData->hasSeenPlayer = true;
		pData->lastKnownPlayerPosition = pPlayer->GetCenter();
		pData->path.clear();
		pEnemy->ClearNavigationTarget();
		return;
	}
	if (pData->hasSeenPlayer)
	{
		if (pData->path.empty())
		{
			CreatePath(*pData, pEnemy);
		}
		FollowPath(*pData, pEnemy);
	}
}

int EnemyNavigator::FindClosestVisibleWayPoint(const Vector2f& position) const
{
	int closestIndex{ -1 };
	float closestDistance{};

	for (const JsonImporter::WayPointInfo& wayPoint : m_WayPoints)
	{
		const bool visible
		{
			m_CollisionManager.HasLineOfSight(
				position,
				wayPoint.position,
				m_Collisions,
				m_Doors
			)
		};

		if (!visible)
		{
			continue;
		}

		const float distance
		{
			(wayPoint.position - position).Length()
		};

		if (closestIndex == -1 ||
			distance < closestDistance)
		{
			closestIndex = wayPoint.index;
			closestDistance = distance;
		}
	}

	return closestIndex;
}

void EnemyNavigator::ResetNavigation(NavigationData& data, Enemy* pEnemy)
{
	data.path.clear();
	data.currentPathIndex = 0;
	data.hasSeenPlayer = false;

	pEnemy->ClearNavigationTarget();
}

EnemyNavigator::NavigationData* EnemyNavigator::FindNavigationData(Enemy* pEnemy)
{
	for (NavigationData& data : m_NavigationData)
	{
		if (data.pEnemy == pEnemy)
		{
			return &data;
		}
	}
	return nullptr;
}

void EnemyNavigator::CreatePath(NavigationData& data, Enemy* pEnemy)
{
	const int startIndex
	{
		FindClosestVisibleWayPoint(
			pEnemy->GetCenter()
		)
	};

	const int targetIndex
	{
		FindClosestVisibleWayPoint(
			data.lastKnownPlayerPosition
		)
	};

	data.path = m_PathFinder.FindPath(startIndex, targetIndex);
	data.currentPathIndex = 0;
}

void EnemyNavigator::FollowPath(NavigationData& data, Enemy* pEnemy)
{
	// All wayPoints completed
	if (data.currentPathIndex >= data.path.size())
	{
		const Vector2f difference = { data.lastKnownPlayerPosition - pEnemy->GetCenter() };

		// Enemy reached player's last known position

		if (difference.Length() <= m_ReachDistance)
		{
			ResetNavigation(data, pEnemy);
			return;
		}
		// After last waypont, go to player's last known pos
		pEnemy->SetNavigationTarget(data.lastKnownPlayerPosition);
		return;
	}
	const int wayPointIndex{ data.path[data.currentPathIndex] };
	const JsonImporter::WayPointInfo* pWayPoint{ m_PathFinder.FindWaypointByIndex(wayPointIndex) };

	if (pWayPoint == nullptr)
	{
		data.path.clear();
		data.currentPathIndex = 0;
		data.hasSeenPlayer = false;

		pEnemy->ClearNavigationTarget();

		return;
	}
	const Vector2f difference{ pWayPoint->position - pEnemy->GetCenter() };

	if (difference.Length() <= m_ReachDistance)
	{
		++data.currentPathIndex;
		return;
	}
	pEnemy->SetNavigationTarget(pWayPoint->position);
}


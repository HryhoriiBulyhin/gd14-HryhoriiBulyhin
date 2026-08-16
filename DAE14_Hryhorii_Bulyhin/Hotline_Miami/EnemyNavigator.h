#pragma once

#include "JsonImporter.h"
#include "PathFinder.h"
#include "CollisionManager.h"

class Enemy;
class Player;

class EnemyNavigator final
{
public:
	explicit EnemyNavigator(const std::vector<JsonImporter::WayPointInfo>& wayPoints, const CollisionManager& collisionManager, const std::vector<Rectf>& collisions, const std::vector<Door*>& doors);
	void Update(Enemy* pEnemy, Player* pPlayer, float elapsedSec);

	int FindClosestVisibleWayPoint(const Vector2f& position)const;

private:
	struct NavigationData
	{
		Enemy* pEnemy{};
		std::vector<int>path{};
		size_t currentPathIndex{};
		bool hasSeenPlayer{};
		Vector2f lastKnownPlayerPosition{};
	};

	void ResetNavigation(NavigationData& data, Enemy* pEnemy);

	NavigationData* FindNavigationData(Enemy* pEnemy);

	void CreatePath(NavigationData& data, Enemy* pEnemy);
	void FollowPath(NavigationData& data, Enemy* pEnemy);

	PathFinder m_PathFinder;
	std::vector<NavigationData> m_NavigationData{};

	float m_ReachDistance{ };

	const std::vector<JsonImporter::WayPointInfo>& m_WayPoints;
	const CollisionManager& m_CollisionManager;
	const std::vector<Rectf>& m_Collisions;
	const std::vector<Door*>& m_Doors;
};


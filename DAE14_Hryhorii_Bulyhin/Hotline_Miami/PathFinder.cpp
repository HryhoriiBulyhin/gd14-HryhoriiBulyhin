#include "pch.h"
#include "PathFinder.h"
#include "CollisionManager.h"

#include <iostream>

PathFinder::PathFinder(const std::vector<JsonImporter::WayPointInfo>& wayPoints)
	:
	m_WayPoints{ wayPoints }
{

}

std::vector<int> PathFinder::FindPath(int startIndex, int targetIndex) const
{
	if (startIndex < 0 ||
		targetIndex < 0)
	{
		return {};
	}

	if (FindWaypointByIndex(startIndex) == nullptr ||
		FindWaypointByIndex(targetIndex) == nullptr)
	{
		return {};
	}

	if (startIndex == targetIndex)
	{
		return { startIndex };
	}

	std::vector<int> parentIndices(m_WayPoints.size(), -1);
	std::vector<bool> visited(m_WayPoints.size(), false);

	std::vector<int> queue{};

	int head{ 0 };

	queue.push_back(startIndex);
	visited[startIndex] = true;

	int reachedTargetIndex{ -1 };

	while (head < static_cast<int>(queue.size()))
	{
		const int currentIndex{ queue[head] };
		++head;

		if (currentIndex == targetIndex)
		{
			reachedTargetIndex = currentIndex;
			break;
		}
		const JsonImporter::WayPointInfo* pWayPoint{ FindWaypointByIndex(currentIndex) };

		if (pWayPoint == nullptr)
		{
			continue;
		}

		for (int neighbourIndex : pWayPoint->neighbours)
		{
			std::cout << neighbourIndex << " ";
			if (neighbourIndex < 0 || neighbourIndex >= static_cast<int>(m_WayPoints.size()))
			{
				continue;
			}
			if (!visited[neighbourIndex])
			{
				visited[neighbourIndex] = true;
				parentIndices[neighbourIndex] = currentIndex;

				queue.push_back(neighbourIndex);
			}
		}
		std::cout << '\n';
	}
	std::vector<int>path{};

	if (reachedTargetIndex != -1)
	{
		int currentIndex{ reachedTargetIndex };

		while (currentIndex != -1)
		{
			path.push_back(currentIndex);

			if (currentIndex == startIndex)
			{
				break;
			}
			currentIndex = parentIndices[currentIndex];
		}
		std::reverse(path.begin(), path.end());
	}
	return path;
}

const JsonImporter::WayPointInfo* PathFinder::FindWaypointByIndex(int index) const
{
	for (const JsonImporter::WayPointInfo& wayPoint : m_WayPoints)
	{
		if (wayPoint.index == index)
		{
			return &wayPoint;
		}
	}
	return nullptr;
}
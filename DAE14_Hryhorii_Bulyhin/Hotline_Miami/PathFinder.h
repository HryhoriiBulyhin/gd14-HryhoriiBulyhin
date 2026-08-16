#pragma once
#include "JsonImporter.h"

class PathFinder final
{
public:
	explicit PathFinder(const std::vector<JsonImporter::WayPointInfo>& wayPoints);

	std::vector<int> FindPath(int startIndex, int targetIndex)const;
	const JsonImporter::WayPointInfo* FindWaypointByIndex(int index)const;
private:
	std::vector<JsonImporter::WayPointInfo> m_WayPoints{};
};


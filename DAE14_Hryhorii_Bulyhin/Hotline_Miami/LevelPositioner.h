#pragma once
#include "JsonImporter.h"
class LevelPositioner
{
public:
	LevelPositioner() = delete;
	static void ApplyOffset(JsonImporter::GameData& levelData, const Vector2f& offset);
private:
	// Have to do it manually
	static void OffsetCollision(JsonImporter::GameData& levelData, const Vector2f& offset);
	static void OffsetPlayerSpawn(JsonImporter::GameData& levelData, const Vector2f& offset);
	static void OffsetCar(JsonImporter::GameData& levelData, const Vector2f& offset);
	static void OffsetEnemies(JsonImporter::GameData& levelData, const Vector2f& offset);
	static void OffsetDoors(JsonImporter::GameData& levelData, const Vector2f& offset);
	static void OffsetLevelExit(JsonImporter::GameData& levelData, const Vector2f& offset);
	static void OffsetWayPoints(JsonImporter::GameData& levelData, const Vector2f& offset);
	static void OffsetSigns(JsonImporter::GameData& levelData, const Vector2f& offset);
};

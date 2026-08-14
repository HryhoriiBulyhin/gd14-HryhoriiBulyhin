#include "pch.h"
#include "LevelPositioner.h"

void LevelPositioner::ApplyOffset(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	OffsetCollision(levelData, offset);
	OffsetPlayerSpawn(levelData, offset);
	OffsetCar(levelData, offset);
	OffsetEnemies(levelData, offset);
	OffsetDoors(levelData, offset);
	OffsetLevelExit(levelData, offset);
	OffsetWayPoints(levelData, offset);
	OffsetSigns(levelData, offset);
}

void LevelPositioner::OffsetCollision(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	for (size_t i = 0; i < levelData.collisions.size(); ++i)
	{
		levelData.collisions[i].left += offset.x;
		levelData.collisions[i].bottom += offset.y;
	}
}

void LevelPositioner::OffsetPlayerSpawn(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	if (!levelData.hasPlayerSpawn)
	{
		return;
	}
	levelData.playerSpawn.x += offset.x;
	levelData.playerSpawn.y += offset.y;
}

void LevelPositioner::OffsetCar(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	if (!levelData.hasCar)
	{
		return;
	}
	levelData.carPosition.x += offset.x;
	levelData.carPosition.y += offset.y;
}
void LevelPositioner::OffsetEnemies(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	for (size_t i = 0; i < levelData.enemies.size(); ++i)
	{
		levelData.enemies[i].position.x += offset.x;
		levelData.enemies[i].position.y += offset.y;
	}
}
void LevelPositioner::OffsetDoors(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	for (size_t i = 0; i < levelData.doors.size(); ++i)
	{
		levelData.doors[i].position.x += offset.x;
		levelData.doors[i].position.y += offset.y;
	}
}
void LevelPositioner::OffsetLevelExit(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	if (!levelData.hasLevelExit)
	{
		return;
	}

	levelData.levelExit.position.x += offset.x;
	levelData.levelExit.position.y += offset.y;

	levelData.levelExit.collider.left += offset.x;
	levelData.levelExit.collider.bottom += offset.y;
}

void LevelPositioner::OffsetWayPoints(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	for (JsonImporter::WayPointInfo& wayPoints : levelData.wayPoints)
	{
		wayPoints.position.x += offset.x;
		wayPoints.position.y += offset.y;
	}
}

void LevelPositioner::OffsetSigns(JsonImporter::GameData& levelData, const Vector2f& offset)
{
	for (JsonImporter::SignInfo& sign : levelData.signs)
	{
		sign.position.x += offset.x;
		sign.position.y += offset.y;
	}
}

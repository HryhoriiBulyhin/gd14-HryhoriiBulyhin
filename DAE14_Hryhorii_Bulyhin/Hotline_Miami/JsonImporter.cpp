#include "pch.h"
#include "JsonImporter.h"

#include <fstream>
#include <iostream>

// Unity uses world units, while the engine uses pixels
// 1 Unity unit corresponds to 100 pixels
const float g_UnityToPixels{ 100.f };

void from_json(const Json& json, Rectf& rect)
{
	rect.left = json.value("left", 0.f);
	rect.bottom = json.value("bottom", 0.f);
	rect.width = json.value("width", 0.f);
	rect.height = json.value("height", 0.f);
}

JsonImporter::GameData JsonImporter::ImportGameInfo(const std::string& jsonPath)
{
	const Json data{ ParseJsonFile(jsonPath) };

	if (data.is_discarded() || data.empty())
	{
		return GameData{};
	}

	GameData processData{};

	try
	{
		if (!data.contains("objects"))
		{
			return processData;
		}
		const Json& objectGroups{ data.at("objects") };

		for (size_t groupindex = 0; groupindex < objectGroups.size(); ++groupindex)
		{
			const Json& objectGroup{ objectGroups.at(groupindex) };

			if (!objectGroup.contains("prog2GameObjects"))
			{
				continue;
			}
			const Json& gameObjects{ objectGroup.at("prog2GameObjects") };

			for (size_t objectIndex = 0; objectIndex < gameObjects.size(); ++objectIndex)
			{
				const Json& object{ gameObjects.at(objectIndex) };

				ProcessJsonObject(object, processData);
			}
		}
	}
	catch (const Json::exception& exception)
	{
		std::cout << "JSON processing ERROR: "
			<< exception.what() << '\n'
			<< "Path: "
			<< jsonPath << '\n';
	}
	return processData;
}

JsonImporter::VisualConfigData JsonImporter::ImportVisualConfig(const std::string& jsonPath)
{
	const Json data{ ParseJsonFile(jsonPath) };

	if (data.is_discarded() || data.empty())
	{
		return VisualConfigData{};
	}
	VisualConfigData visualConfig{};

	try
	{
		if (data.contains("player"))
		{
			const Json& playerObject{ data.at("player") };
			visualConfig.player = ReadPlayerVisualInfo(playerObject);
		}
		if (data.contains("enemy"))
		{
			const Json& enemyObject{ data.at("enemy") };
			visualConfig.enemy = ReadEnemyVisualInfo(enemyObject);
		}
		if (data.contains("mouse"))
		{
			const Json& mousObject{ data.at("mouse") };
			visualConfig.mouse = ReadAnimationInfo(mousObject);
		}
	}
	catch (const Json::exception& exception)
	{
		std::cout << "Visual config JSON processing ERROR: "
			<< exception.what() << '\n'
			<< "Path: "
			<< jsonPath << '\n';
	}
	return visualConfig;
}

JsonImporter::HitboxConfigData JsonImporter::ImoprtHitboxConfig(const std::string& jsonPath)
{
	const Json data{ ParseJsonFile(jsonPath) };

	if (data.is_discarded() || data.empty())
	{
		return HitboxConfigData{};
	}

	HitboxConfigData hitboxConfig{};

	try
	{
		if (data.contains("player") && !data.at("player").is_null())
		{
			hitboxConfig.player = ReadEntityHitboxInfo(data.at("player"));
		}
		if (data.contains("enemy") && !data.at("enemy").is_null())
		{
			hitboxConfig.enemy = ReadEntityHitboxInfo(data.at("enemy"));
		}
	}
	catch (const Json::exception& exception)
	{
		std::cout << "Hitbox config JSON --> ERROR: "
			<< exception.what() << '\n'
			<< "Path: "
			<< jsonPath << '\n';
	}
	return hitboxConfig;
}

Json JsonImporter::ParseJsonFile(const std::string& jsonPath)
{
	std::ifstream inputFile{ jsonPath };

	if (!inputFile.is_open())
	{
		std::cout << "Could not open JSON FILE: "
			<< jsonPath << '\n';

		return Json::value_t::discarded;
	}
	Json parseData{};

	try
	{
		inputFile >> parseData;
	}
	catch (const Json::parse_error& exception)
	{
		std::cout << "JSON parse ERROR: "
			<< exception.what()
			<< '\n';

		return Json::value_t::discarded;
	}
	return parseData;
}

void JsonImporter::ProcessJsonObject(const Json& object, GameData& destination)
{
	const std::string objectType{ object.value("objectType",std::string{}) };

	if (objectType == "Background" || object.contains("texturePath") && objectType.empty())
	{
		AddBackground(object, destination);
	}
	else if (objectType == "Collision")
	{
		AddCollision(object, destination);
	}
	else if (objectType == "PlayerSpawn")
	{
		AddPlayerSpawn(object, destination);
	}
	else if (objectType == "Car")
	{
		AddCar(object, destination);
	}
	else if (objectType == "Enemy")
	{
		AddEnemy(object, destination);
	}
	else if (objectType == "Door")
	{
		AddDoor(object, destination);
	}
	else if (objectType == "LevelExit")
	{
		AddLevelExit(object, destination);
	}
	else if (objectType == "Waypoint")
	{
		AddWayPoint(object, destination);
	}
	else if (objectType == "Sign")
	{
		AddSign(object, destination);
	}
	else if (objectType.find("EnemyPatrol_") == 0)
	{
		AddPatrolPoint(object, destination);
	}
}

void JsonImporter::AddBackground(const Json& object, GameData& destination)
{
	destination.backgroundTexturePath = object.value("texturePath", std::string{});
	destination.backgroundRenderLayer = object.value("renderLayer", 0);

	const std::string objectType{ object.value("objectType",std::string{}) };

	if (objectType == "Background")
	{
		destination.coordinateMode = CoordinateMode::PixelBottomLeft;
		destination.backgroundPosition.Set(object.value("xPosition", 0.f), object.value("yPosition", 0.f));
	}
	else
	{
		destination.coordinateMode = CoordinateMode::LegacyUnityUnits;
		destination.backgroundPosition.Set(object.value("xPosition", 0.f) * g_UnityToPixels,
			object.value("yPosition", 0.f) * g_UnityToPixels);
	}
}

void JsonImporter::AddCollision(const Json& object, GameData& destination)
{
	if (!object.contains("prog2Rectf"))
	{
		return;
	}

	const float coordinateScale{ GetCoordinateScale(destination) };

	const float objectX
	{
		ConvertX(object.value("xPosition",0.f), destination)
	};
	const float objectY
	{
		ConvertY(object.value("yPosition",0.f), destination)
	};

	const Rectf localCollider{ object.at("prog2Rectf").get<Rectf>() };

	const Rectf worldCollider
	{
		objectX + localCollider.left * coordinateScale,
		objectY + localCollider.bottom * coordinateScale,
		localCollider.width * coordinateScale,
		localCollider.height * coordinateScale
	};
	destination.collisions.push_back(worldCollider);
}

void JsonImporter::AddPlayerSpawn(const Json& object, GameData& destination)
{
	destination.playerSpawn.Set(ConvertX(object.value("xPosition", 0.f), destination),
		ConvertY(object.value("yPosition", 0.f), destination));
	destination.hasPlayerSpawn = true;
}

void JsonImporter::AddCar(const Json& object, GameData& destination)
{
	destination.carPosition.Set(ConvertX(object.value("xPosition", 0.f), destination),
		ConvertY(object.value("yPosition", 0.f), destination));

	destination.hasCar = true;
}

void JsonImporter::AddEnemy(const Json& object, GameData& destination)
{
	EnemyInfo enemy{};

	enemy.subType = object.value("subType", std::string{});
	enemy.position.Set(ConvertX(object.value("xPosition", 0.f), destination),
		ConvertY(object.value("yPosition", 0.f), destination));
	enemy.groupId = object.value("groupId", 0);
	enemy.index = object.value("index", 0);
	destination.enemies.push_back(enemy);
}

void JsonImporter::AddDoor(const Json& object, GameData& destination)
{
	DoorInfo door{};

	door.subType = object.value("subType", std::string{});
	door.position.Set(ConvertX(object.value("xPosition", 0.f), destination),
		ConvertY(object.value("yPosition", 0.f), destination));

	if (object.contains("prog2Rectf"))
	{
		const float coordingateScale{ GetCoordinateScale(destination) };

		const Rectf localRect{ object.at("prog2Rectf").get<Rectf>() };

		door.rect = Rectf
		{
			localRect.left * coordingateScale,
			localRect.bottom * coordingateScale,
			localRect.width * coordingateScale,
			localRect.height * coordingateScale
		};
	}
	door.groupId = object.value("groupId", 0);
	door.index = object.value("index", 0);
	destination.doors.push_back(door);
}

void JsonImporter::AddLevelExit(const Json& object, GameData& destination)
{
	destination.levelExit.position.Set(ConvertX(object.value("xPosition", 0.f), destination),
		ConvertY(object.value("yPosition", 0.f), destination));

	destination.levelExit.isTrigger = object.value("isTrigger", false);

	if (object.contains("prog2Rectf"))
	{
		const float coordinateScale{ GetCoordinateScale(destination) };

		const Rectf localCollider{ object.at("prog2Rectf").get<Rectf>() };

		destination.levelExit.collider = Rectf
		{
			destination.levelExit.position.x + localCollider.left * coordinateScale,
			destination.levelExit.position.y + localCollider.bottom * coordinateScale,
			localCollider.width * coordinateScale,
			localCollider.height * coordinateScale
		};
	}
	destination.hasLevelExit = true;
}

void JsonImporter::AddWayPoint(const Json& object, GameData& destination)
{
	WayPointInfo wayPoint{};

	wayPoint.position.Set(ConvertX(object.value("xPosition", 0.f), destination), ConvertY(object.value("yPosition", 0.f), destination));
	wayPoint.groupId = object.value("groupId", 0);
	wayPoint.index = object.value("index", 0);

	if (object.contains("neighbours") && object.at("neighbours").is_array())
	{
		const Json& neighbours{ object.at("neighbours") };

		for (const Json& neighbour : neighbours)
		{
			wayPoint.neighbours.push_back(neighbour.get<int>());
		}
	}

	destination.wayPoints.push_back(wayPoint);
}

void JsonImporter::AddSign(const Json& object, GameData& destination)
{
	SignInfo sign{};

	sign.position.Set(ConvertX(object.value("xPosition", 0.f), destination),
		ConvertY(object.value("yPosition", 0.f), destination)
	);

	sign.subType = object.value("subType", std::string{});
	destination.signs.push_back(sign);
}

void JsonImporter::AddPatrolPoint(const Json& object, GameData& destination)
{
	PatrolPointInfo patrolPoint{};

	patrolPoint.position.Set(ConvertX(object.value("xPosition", 0.f), destination),
		ConvertY(object.value("yPosition", 0.f), destination));

	patrolPoint.patrolId = object.value("objectType", std::string{});
	patrolPoint.index = object.value("index", 0);
	destination.patrolPoints.push_back(patrolPoint);
}

float JsonImporter::ConvertX(float x, const GameData& destination)
{
	if (destination.coordinateMode == CoordinateMode::PixelBottomLeft)
	{
		return x - destination.backgroundPosition.x;
	}
	return x * g_UnityToPixels - destination.backgroundPosition.x;
}

float JsonImporter::ConvertY(float y, const GameData& destination)
{
	if (destination.coordinateMode == CoordinateMode::PixelBottomLeft)
	{
		return y - destination.backgroundPosition.y;
	}
	return y * g_UnityToPixels - destination.backgroundPosition.y;
}

float JsonImporter::GetCoordinateScale(const GameData& destination)
{
	if (destination.coordinateMode == CoordinateMode::PixelBottomLeft)
	{
		return 1.f;
	}
	return g_UnityToPixels;
}

JsonImporter::AnimationVisualInfo JsonImporter::ReadAnimationInfo(const Json& object)
{
	AnimationVisualInfo animation{};

	animation.texturePath = object.value("texturePath", std::string{});
	animation.rows = object.value("rows", 0);
	animation.cols = object.value("columns", 0);
	animation.frameCount = object.value("frameCount", 0);
	animation.frameTime = object.value("frameTime", 0.f);
	animation.scale = object.value("scale", 1.f);
	animation.loop = object.value("loop", false);

	return animation;
}

JsonImporter::VisualPresetInfo JsonImporter::ReadVisualPresetInfo(const Json& object)
{
	VisualPresetInfo preset{};
	preset.id = object.value("id", std::string{});

	if (object.contains("walk") && !object.at("walk").is_null())
	{
		preset.walk = ReadAnimationInfo(object.at("walk"));
		preset.hasWalk = !preset.walk.texturePath.empty();
	}
	if (object.contains("attack") && !object.at("attack").is_null())
	{
		preset.attack = ReadAnimationInfo(object.at("attack"));
		preset.hasAttack = !preset.attack.texturePath.empty();
	}
	if (object.contains("dead") && !object.at("dead").is_null())
	{
		preset.dead = ReadAnimationInfo(object.at("dead"));
		preset.hasDead = true;
	}
	return preset;
}

JsonImporter::PlayerVisualInfo JsonImporter::ReadPlayerVisualInfo(const Json& object)
{
	PlayerVisualInfo player{};

	if (object.contains("dead"))
	{
		const Json& deadObject{ object.at("dead") };
		player.dead = ReadAnimationInfo(deadObject);
	}
	if (object.contains("finisher"))
	{
		const Json& finisherObject{ object.at("finisher") };
		player.finisher = ReadAnimationInfo(finisherObject);
	}
	if (object.contains("presets"))
	{
		const Json& presets{ object.at("presets") };

		for (size_t presetIndex = 0; presetIndex < presets.size(); ++presetIndex)
		{
			const Json& presetObject{ presets.at(presetIndex) };
			const VisualPresetInfo preset{ ReadVisualPresetInfo(presetObject) };
			player.presets.push_back(preset);
		}
	}
	return player;
}

JsonImporter::EnemyVisualInfo JsonImporter::ReadEnemyVisualInfo(const Json& object)
{
	EnemyVisualInfo enemy{};

	if (object.contains("dead"))
	{
		const Json& deadObject{ object.at("dead") };
		enemy.dead = ReadAnimationInfo(deadObject);
	}
	if (object.contains("presets"))
	{
		const Json& presets{ object.at("presets") };

		for (size_t presetIndex = 0; presetIndex < presets.size(); ++presetIndex)
		{
			const Json& presetObject{ presets.at(presetIndex) };

			const VisualPresetInfo preset{ ReadVisualPresetInfo(presetObject) };
			enemy.presets.push_back(preset);

		}
	}
	return enemy;
}

JsonImporter::HitboxFrameInfo JsonImporter::ReadHitboxInfo(const Json& object)
{
	HitboxFrameInfo frame{};

	frame.name = object.value("name", std::string{});
	frame.x = object.value("x", 0.f);
	frame.y = object.value("y", 0.f);
	frame.width = object.value("width", 0.f);
	frame.height = object.value("height", 0.f);

	return frame;
}

JsonImporter::AnimationHitboxInfo JsonImporter::ReadAnimationHitboxInfo(const Json& object)
{
	AnimationHitboxInfo animation{};

	if (object.is_null())
	{
		return animation;
	}
	animation.texturePath = object.value("texturePath", std::string{});

	if (!object.contains("frames") || !object.at("frames").is_array())
	{
		return animation;
	}
	const Json& frames{ object.at("frames") };

	animation.frames.reserve(frames.size());

	for (const Json& frameObject : frames)
	{
		animation.frames.push_back(ReadHitboxInfo(frameObject));
	}
	return animation;
}

JsonImporter::HitboxPresetInfo JsonImporter::ReadHitboxPresetInfo(const Json& object)
{
	HitboxPresetInfo preset{};

	preset.id = object.value("id", std::string{});

	if (object.contains("walk") && !object.at("walk").is_null())
	{
		preset.walk = ReadAnimationHitboxInfo(object.at("walk"));
		preset.hasWalk = preset.walk.IsValid();
	}

	if (object.contains("attack") && !object.at("attack").is_null())
	{
		preset.attack = ReadAnimationHitboxInfo(object.at("attack"));
		preset.hasAttack = preset.attack.IsValid();
	}
	return preset;
}

JsonImporter::EntityHitboxInfo JsonImporter::ReadEntityHitboxInfo(const Json& object)
{
	EntityHitboxInfo entity{};

	if (!object.contains("presets") || !object.at("presets").is_array())
	{
		return entity;
	}
	const Json& presets{ object.at("presets") };
	entity.presets.reserve(presets.size());

	for (const Json& presetObject : presets)
	{
		entity.presets.push_back(ReadHitboxPresetInfo(presetObject));
	}
	return entity;
}
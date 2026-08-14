#pragma once
#include <string>
#include <vector>

using Json = nlohmann::json;
void from_json(const Json& json, Rectf& rect);

class JsonImporter final
{
public:
	enum class CoordinateMode
	{
		LegacyUnityUnits,
		PixelBottomLeft
	};
	struct EnemyInfo final
	{
		std::string subType{};
		Vector2f position{};
		int groupId{};
		int index{};
	};
	struct DoorInfo final
	{
		std::string subType{};
		Vector2f position{};
		Rectf rect{};
		int groupId{};
		int index{};
	};
	struct WayPointInfo final
	{
		Vector2f position{};
		int groupId{};
		int index{};

		std::vector<int> neighbours{};
	};
	struct LevelExitInfo final
	{
		Vector2f position{};
		Rectf collider{};
		bool isTrigger{};
	};
	struct SignInfo
	{
		Vector2f position{};
		std::string subType{};
	};
	struct PatrolPointInfo final
	{
		Vector2f position{};
		std::string patrolId{};
		int index{};
	};
	struct GameData final
	{
		CoordinateMode coordinateMode{ CoordinateMode::LegacyUnityUnits };

		std::string backgroundTexturePath{};
		Vector2f backgroundPosition{};
		int backgroundRenderLayer{};

		std::vector<Rectf>collisions{};

		Vector2f playerSpawn{};
		bool hasPlayerSpawn{};
		Vector2f carPosition{};
		bool hasCar{};

		std::vector<EnemyInfo> enemies{};
		std::vector<DoorInfo> doors{};
		std::vector<WayPointInfo> wayPoints{};
		std::vector<SignInfo> signs{};
		std::vector<PatrolPointInfo> patrolPoints{};

		LevelExitInfo levelExit{};
		bool hasLevelExit{};
	};
	struct AnimationVisualInfo final
	{
		std::string texturePath{};
		int rows{};
		int cols{};
		int frameCount{};
		float frameTime{};
		float scale{};
		bool loop{};
	};
	struct VisualPresetInfo final
	{
		std::string id{};
		AnimationVisualInfo walk{};
		AnimationVisualInfo attack{};
		AnimationVisualInfo dead{};
		bool hasWalk{};
		bool hasAttack{};
		bool hasDead{};
	};
	struct PlayerVisualInfo final
	{
		AnimationVisualInfo dead{};
		AnimationVisualInfo finisher{};
		std::vector<VisualPresetInfo> presets{};
	};
	struct EnemyVisualInfo final
	{
		AnimationVisualInfo dead{};
		std::vector<VisualPresetInfo> presets{};
	};
	struct VisualConfigData final
	{
		PlayerVisualInfo player{};
		EnemyVisualInfo enemy{};
		AnimationVisualInfo mouse{};
	};
	struct HitboxFrameInfo final
	{
		std::string name{};

		float x{};
		float y{};
		float width{};
		float height{};
	};
	struct AnimationHitboxInfo final
	{
		std::string texturePath{};
		std::vector<HitboxFrameInfo> frames{};

		bool IsValid() const
		{
			return !frames.empty();
		}
	};
	struct HitboxPresetInfo final
	{
		std::string id{};

		AnimationHitboxInfo walk{};
		AnimationHitboxInfo attack{};

		bool hasWalk{};
		bool hasAttack{};
	};
	struct EntityHitboxInfo final
	{
		std::vector<HitboxPresetInfo> presets{};
	};
	struct HitboxConfigData final
	{
		EntityHitboxInfo player{};
		EntityHitboxInfo enemy{};
	};
	JsonImporter() = delete;

	static GameData ImportGameInfo(const std::string& jsonPath);
	static VisualConfigData ImportVisualConfig(const std::string& jsonPath);
	static HitboxConfigData ImoprtHitboxConfig(const std::string& jsonPath);
private:
	static Json ParseJsonFile(const std::string& jsonPath);
	static void ProcessJsonObject(const Json& object, GameData& destination);
	static void AddBackground(const Json& object, GameData& destination);
	static void AddCollision(const Json& object, GameData& destination);
	static void AddPlayerSpawn(const Json& object, GameData& destination);
	static void AddCar(const Json& object, GameData& destination);
	static void AddEnemy(const Json& object, GameData& destination);
	static void AddDoor(const Json& object, GameData& destination);
	static void AddLevelExit(const Json& object, GameData& destination);
	static void AddWayPoint(const Json& object, GameData& destination);
	static void AddSign(const Json& object, GameData& destination);
	static void AddPatrolPoint(const Json& object, GameData& destination);

	static float ConvertX(float x, const GameData& destination);
	static float ConvertY(float y, const GameData& destination);
	static float GetCoordinateScale(const GameData& destination);

	static AnimationVisualInfo ReadAnimationInfo(const Json& object);
	static VisualPresetInfo ReadVisualPresetInfo(const Json& object);
	static PlayerVisualInfo ReadPlayerVisualInfo(const Json& object);
	static EnemyVisualInfo ReadEnemyVisualInfo(const Json& object);

	static HitboxFrameInfo ReadHitboxInfo(const Json& object);
	static AnimationHitboxInfo ReadAnimationHitboxInfo(const Json& object);
	static HitboxPresetInfo ReadHitboxPresetInfo(const Json& object);
	static EntityHitboxInfo ReadEntityHitboxInfo(const Json& object);
};
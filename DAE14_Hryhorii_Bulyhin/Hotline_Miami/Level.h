#pragma once
#include <string>

#include "JsonImporter.h"
#include "CollisionManager.h"
#include "WeaponType.h"
#include "DroppedItem.h"
#include "Corpse.h"

#include <vector>

class Texture;
class Entity;
class Enemy;
class DroppedItem;
class Player;
class ResourceManager;
class Door;
class EnemyNavigator;
class Sign;
class Car;
class Level final
{
public:
    explicit Level(const std::string& jsonPath);
    ~Level();

    Level(const Level& other) = delete;
    Level& operator=(const Level& other) = delete;
    Level(Level&& other) = delete;
    Level& operator=(Level&& other) = delete;

    void Draw() const;
    void DrawDroppedItems()const;
    void DrawCar()const;
    void DrawDoors()const;
    void Update(float elapsedSec);

    const JsonImporter::GameData& GetData() const;
    JsonImporter::GameData& GetData();
    void LoadEntities(std::vector<Enemy*>& enemies, const JsonImporter::EntityHitboxInfo& hitboxInfo, const JsonImporter::EnemyVisualInfo& visualInfo,
        ResourceManager* pResources, bool loadEnemies = true);

    bool IsEntityColliding(const Entity& entity)const;
    bool ResolveEntityMovement(Entity& entity, const Vector2f& previousPos)const;
    void SetPlayer(Player* pPlayer);

    const std::vector<Door*>& GetDoors()const;
    Car* GetCar()const;
#pragma region DroppedItem
    void UpdateEnemyWeaponDrops();
    void UpdateEnemyWeaponSearch();
    void UpdateEnemyWeaponPickUp();
    void RemovePickedUpItems();
    void TryPickUpDroppedItem();
#pragma endregion DroppedItem
    void TryStartFinisher();

    bool AreAllEnemiesDead()const;
    bool IsPlayerOnLevelExit()const;
    Vector2f GetLevelExitSignPosition()const;
#pragma region Coprses
    std::vector<Corpse::CorpseData> GetCorpseData()const;
    void LoadCorpses(const std::vector<Corpse::CorpseData>& corpseData);
#pragma endregion Corpses
private:
    void Load(const std::string& jsonPath);
    void LoadDoors();
    void LoadCar();
    void PositionLevelData();
    void ScaleLevelData();
    bool CanEnemySeePlayer(const Enemy& enemy)const;

    void UpdateLevelExitSign();
    std::vector<Vector2f> GetPatrolPoints(int enemyIndex)const;

    void Clear();

    JsonImporter::GameData m_Data{};
    Texture* m_pBackground{};

    CollisionManager m_CollisionManager{};

    float m_LevelScale{  };

    std::vector<DroppedItem*> m_DroppedItems{};
    std::vector<Enemy*> m_Enemies{};
    std::vector<Enemy*> m_EnemiesThatDroppedItem{};
    Player* m_pPlayer{};
    ResourceManager* m_pResources{};

    std::vector<Door*> m_Doors{};
    EnemyNavigator* m_pEnemyNavigator{};

    Sign* m_pLevelExitSign{};

    std::vector<Corpse*>m_Corpses{};

    Car* m_pCar{};
};
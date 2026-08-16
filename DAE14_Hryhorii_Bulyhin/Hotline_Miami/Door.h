#pragma once
#include "WorldObject.h"
#include "JsonImporter.h"

class Player;
class Texture;
class Enemy;
class ResourceManager;
class Entity;
class Door final : public WorldObject
{
public:
	explicit Door(const JsonImporter::DoorInfo& doorInfo, ResourceManager* pResources);

	Door(const Door& other) = delete;
	Door& operator=(const Door& other) = delete;
	Door(Door&& other) = delete;
	Door& operator=(Door&& other) = delete;

	void Update(float elapsedSec) override;
	void Draw()const override;

	void Push(const Vector2f& pusherPos);

	void HandlePlayer(Player* pPlayer, const Vector2f& moveDir);
	void HandleEnemy(Enemy* pEnemy, const Vector2f& moveDir);

	bool IsSolid() const;

	const Vector2f* GetCurrentPoints()const;
	int GetVertexCount()const;

	float GetAngle()const;
	float GetAngularVelocity()const;
	bool WasPushedByPlayer()const;
private:
	enum class Orientation
	{
		Horizontal,
		Vertical
	};

	void Load(const JsonImporter::DoorInfo& doorInfo);
	void LoadTextures(ResourceManager* pResources);
	void InitializeVertices(const JsonImporter::DoorInfo& doorInfo);

	void ClampAngle();
	void UpdateVertices();
	void ApplyFriction(float elapsedSec);

	bool IsOverlappingEntity(const Entity& entity)const;

	Vector2f GetPivot()const;

	Texture* m_pTexture{};

	Orientation m_Orientation{ };

	float m_Angle{};
	float m_AngularVelocity{};

	float m_PushForce{};
	float m_FrictionPerSecond{ };

	float m_StopVelocity{};
	float m_MaxAngle{};
	float m_SolidAngle{};

	static const int m_VertexCount{ 4 };

	Vector2f m_LocalVertices[m_VertexCount];
	Vector2f m_WorldVertices[m_VertexCount];

	Rectf m_LocalRect{};

	bool m_WasPushedByPlayer{};
	float m_HitVelocity{};
};


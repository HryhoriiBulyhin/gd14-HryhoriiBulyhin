#pragma once
#include "WorldObject.h"
#include "JsonImporter.h"
#include "Animation.h"
class Texture;
class ResourceManager;
class Player;
class Entity;
class Car final : public WorldObject
{
public:
	enum class CarState
	{
		Closed,
		Opening,
		Open,
		Closing,
		Engine
	};
	explicit Car(const Vector2f& position, ResourceManager* pResources);

	Car(const Car& other) = delete;
	Car& operator=(const Car& other) = delete;
	Car(Car&& other) = delete;
	Car& operator=(Car&& other) = delete;

	void Update(float elapsedSec)override;
	void Draw()const;

	void HandlePlayer(Player* pPLayer);

	CarState GetState() const;
	bool HasStartedEngine()const;

	const Rectf& GetBounds() const;

	bool IsCollidingWithPlayer(const Player& player)const;
private:
	void InitializeHitBoxes();

	void UpdateOpening(float elapsedSec);
	void UpdateClosing(float elapsedSec);
	void UpdateEngine(float elapsedSec);

	Texture* m_pTexture{};

	CarState m_State{};

	int m_FrameCount{};

	int m_OpenFrame{},
		m_ClosedFrame{},
		m_EngineStartFrame{},
		m_EngineEndFrame{};

	float m_Scale{};
	float m_AnimationTime{};
	float m_FrameDuration{ };

	const float m_TriggerWidth{},
		m_EnterHeight{};

	bool m_HasStartedEngine{};

	Rectf m_Bounds{};
	Rectf m_TriggerCollision{};
	Rectf m_EnterCollision{};

	Animation m_Animation{};

	Vector2f m_PlayerPos{};
};


#pragma once
#include "WorldObject.h"
#include "Animation.h"
#include "JsonImporter.h"

class Texture;
class ResourceManager;

class Sign final : public WorldObject
{
public:
	explicit Sign(const JsonImporter::SignInfo& signInfo, ResourceManager* pResources);

	Sign(const Sign& other) = delete;
	Sign& operator=(const Sign& other) = delete;
	Sign(Sign&& other) = delete;
	Sign& operator=(Sign&& other) = delete;

	void Update(float elapsedSec)override;
	void Draw()const override;

private:
	Texture* m_pSignTexture{};

	Animation m_Animation{};

	Vector2f m_DrawPosition{};

	float m_SmoothTime{},
		m_SmoothSpeed{},
		m_SmoothDistance{};

	float m_Scale{};
};


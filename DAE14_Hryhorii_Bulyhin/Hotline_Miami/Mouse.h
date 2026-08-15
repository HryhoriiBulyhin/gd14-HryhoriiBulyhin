#pragma once
#include "Animation.h"
#include "JsonImporter.h"
class ResourceManager;
class Texture;
class Mouse
{
public:

	explicit Mouse(const JsonImporter::AnimationVisualInfo& visualInfo, ResourceManager* pResources);

	Mouse(Mouse&& other) = delete;
	Mouse& operator=(Mouse&& other) = delete;

	Mouse(const Mouse& other) = delete;
	Mouse& operator=(const Mouse& other) = delete;

	void Update(const Rectf& viewport, const Vector2f& cameraPos, float elapsedSec);
	void Draw() const;

	const Vector2f& GetScreenPos() const;
	const Vector2f& GetWorldPos() const;

private:
	Vector2f m_ScreenPos{};
	Vector2f m_WorldPos{};

	Animation m_Animation{};

	Texture* m_pMouseTexture{};
	float m_Scale{};
};


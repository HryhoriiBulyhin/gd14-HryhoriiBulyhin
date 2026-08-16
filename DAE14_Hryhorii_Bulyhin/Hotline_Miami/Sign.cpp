#include "pch.h"
#include "Sign.h"
#include "Texture.h"
#include "ResourceManager.h"
Sign::Sign(const JsonImporter::SignInfo& signInfo, ResourceManager* pResources)
	:
	WorldObject{ signInfo.position },
	m_SmoothSpeed{ 3.f },
	m_SmoothDistance{ 4.f },
	m_Scale{ 2.5f },
	m_DrawPosition{ signInfo.position }
{
	std::string texturePath{ "Entity/WorldObject/sprGoArrow_strip4.png" };
	int rows{ 1 },
		cols{ 4 },
		frameCount{ 4 };
	float frameTime{ 0.12f };

	m_pSignTexture = pResources->GetTexture(texturePath);

	m_Animation = Animation
	{
		rows,
		cols,
		frameCount,
		frameTime,
		false
	};

	if (signInfo.subType == "LevelExit")
	{
		m_Animation.SetStartFrame(1);
	}
}

void Sign::Update(float elapsedSec)
{
	m_SmoothTime += elapsedSec;

	const float offsetY
	{
		static_cast<float>(std::sin(m_SmoothTime * m_SmoothSpeed) * m_SmoothDistance)
	};
	m_DrawPosition = Vector2f{ GetPosition().x, GetPosition().y + offsetY };
}

void Sign::Draw() const
{
	const float frameWidth{ m_pSignTexture->GetWidth() / static_cast<float>(m_Animation.GetColumns()) };
	const float frameHeight{ m_pSignTexture->GetHeight() / static_cast<float>(m_Animation.GetRows()) };

	const Rectf srcRect
	{
		m_Animation.GetCurrentColumn() * frameWidth,
		m_Animation.GetCurrentRow() * frameHeight,
		frameWidth,
		frameHeight
	};
	const Rectf dstRect
	{
		m_DrawPosition.x - frameWidth * m_Scale / 2.f,
		m_DrawPosition.y - frameHeight * m_Scale / 2.f,
		frameWidth * m_Scale,
		frameHeight * m_Scale
	};
	m_pSignTexture->Draw(dstRect, srcRect);
}

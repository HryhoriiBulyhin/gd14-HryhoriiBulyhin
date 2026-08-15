#include "pch.h"
#include "Mouse.h"
#include "Texture.h"
#include "ResourceManager.h"

Mouse::Mouse(const JsonImporter::AnimationVisualInfo& visualInfo, ResourceManager* pResources)
	:
	m_Animation
	{
		visualInfo.rows,
		visualInfo.cols,
		visualInfo.frameCount,
		visualInfo.frameTime,
		visualInfo.loop
	},
	m_pMouseTexture{ pResources->GetTexture(visualInfo.texturePath) },
	m_Scale{ visualInfo.scale }
{
}
void Mouse::Update(const Rectf& viewport, const Vector2f& cameraPos, float elapsedSec)
{
	int x{}, y{};
	SDL_GetMouseState(&x, &y);

	m_ScreenPos.x = static_cast<float>(x);
	m_ScreenPos.y = viewport.height - static_cast<float>(y);

	m_WorldPos.x = m_ScreenPos.x + cameraPos.x;
	m_WorldPos.y = m_ScreenPos.y + cameraPos.y;

	m_Animation.Update(elapsedSec);
}
void Mouse::Draw() const
{
	float frameWidth = m_pMouseTexture->GetWidth() / static_cast<float>(m_Animation.GetColumns());
	float frameHeight = m_pMouseTexture->GetHeight() / static_cast<float>(m_Animation.GetRows());

	Rectf srcRect
	{
		m_Animation.GetCurrentColumn() * frameWidth,
		m_Animation.GetCurrentRow() * frameHeight,
		frameWidth,
		frameHeight
	};
	Rectf dstRect
	{
		m_ScreenPos.x - frameWidth * m_Scale / 2.f,
		m_ScreenPos.y - frameHeight * m_Scale / 2.f,
		frameWidth * m_Scale,
		frameHeight * m_Scale
	};

	m_pMouseTexture->Draw(dstRect, srcRect);
}

const Vector2f& Mouse::GetScreenPos() const
{
	return m_ScreenPos;
}

const Vector2f& Mouse::GetWorldPos() const
{
	return m_WorldPos;
}

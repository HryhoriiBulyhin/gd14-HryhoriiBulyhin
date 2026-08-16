#include "pch.h"
#include "Corpse.h"
#include "ResourceManager.h"
#include "Texture.h"

Corpse::Corpse(const CorpseData& data, ResourceManager* pResources)
	:
	m_Data{ data },
	m_Scale{ 2.f }
{
	if (m_Data.weaponType == WeaponType::Case)
	{
		std::string texturePath{ "Entity/Enemy/sprESubwayDead_strip6.png" };

		m_pTexture = pResources->GetTexture(texturePath);

		m_Cols = 6;
		m_FrameIndex = 5;
	}
	else
	{
		std::string texturePath{ "Entity/Enemy/sprEAxeKill_strip4.png" };
		m_pTexture = pResources->GetTexture(texturePath);
		m_Cols = 4;
		m_FrameIndex = 3;
	}
}
void Corpse::Draw() const
{
	const float frameWidth
	{
		m_pTexture->GetWidth() / static_cast<float>(m_Cols)
	};
	const float frameHeight
	{
		m_pTexture->GetHeight()
	};

	const Rectf srcRect
	{
		m_FrameIndex * frameWidth,
		0.f,
		frameWidth,
		frameHeight
	};
	const float drawWidth{ frameWidth * m_Scale };
	const float drawHeight{ frameHeight * m_Scale };

	const Rectf localDstRect
	{
		-drawWidth / 2.f,
		-drawHeight / 2.f,
		drawWidth,
		drawHeight
	};

	glPushMatrix();
	{
		glTranslatef(m_Data.position.x, m_Data.position.y, 0.f);

		glRotatef(m_Data.rotationAngle, 0.f, 0.f, 1.f);
		m_pTexture->Draw(localDstRect, srcRect);
	}
	glPopMatrix();
}
const Corpse::CorpseData& Corpse::GetData() const
{
	return m_Data;
}
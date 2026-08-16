#include "pch.h"
#include "DroppedItem.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Player.h"
#include "Enemy.h"
#include "utils.h"

Vector2f DroppedItem::GetDropPosition(Enemy* pOwner)
{
	return pOwner->GetCenter();
}

DroppedItem::DroppedItem(Enemy* pOwner, Player* pPlayer, WeaponType weaponType, ResourceManager* pResources)
	:
	WorldObject{ GetDropPosition(pOwner) },
	m_pOwner{ pOwner },
	m_pPlayer{ pPlayer },
	m_WeaponType{ weaponType },
	m_Scale{ 1.5f },
	m_DropOffset{ 20.f },
	m_IsPickedUp{ false }
{
	switch (m_WeaponType)
	{
	case WeaponType::Bat:
	{
		m_pItemTexture = pResources->GetTexture("Entity/WorldObject/sprBigWeapons_strip23.png");
		m_Columns = 23;
		m_FrameIndex = 2;
		break;
	}
	case WeaponType::Knife:
	{
		m_pItemTexture = pResources->GetTexture("Entity/WorldObject/sprBigWeapons_strip23.png");
		m_Columns = 23;
		m_FrameIndex = 3;
		break;
	}
	case WeaponType::Case:
	{
		m_pItemTexture = pResources->GetTexture("Entity/WorldObject/sprBriefcase.png");
		m_Columns = 1;
		m_FrameIndex = 0;
		m_Scale = 3.f;
		break;
	}
	}

	const Vector2f itemPosition{ GetPosition().x + m_DropOffset, GetPosition().y };
	SetPosition(itemPosition);

	const float frameWidth{ m_pItemTexture->GetWidth() / static_cast<float>(m_Columns) };
	const float frameHeight{ m_pItemTexture->GetHeight() };

	const Rectf localRect
	{
		-frameWidth / 2.f,
		-frameHeight / 2.f,
		frameWidth,
		frameHeight
	};
	m_PickUpHitBox.SetLocalRect(localRect);
	m_PickUpHitBox.Update(GetPosition(), m_Scale, 0);
}
void DroppedItem::Update(float elapsedSec)
{
	if (m_IsPickedUp)
	{
		return;
	}
}

void DroppedItem::Draw() const
{
	if (m_IsPickedUp)
	{
		return;
	}
	const float frameWidth{ m_pItemTexture->GetWidth() / static_cast<float>(m_Columns) };
	const float frameHeight{ m_pItemTexture->GetHeight() };

	const Rectf srcRect
	{
		m_FrameIndex * frameWidth,
		0.f,
		frameWidth,
		frameHeight
	};
	const Rectf dstRect
	{
		GetPosition().x - frameWidth * m_Scale / 2.f,
		GetPosition().y - frameHeight * m_Scale / 2.f,
		frameWidth * m_Scale,
		frameHeight * m_Scale
	};
	m_pItemTexture->Draw(dstRect, srcRect);
}


void DroppedItem::GiveToPlayer()
{
	m_pPlayer->SetWeaponType(m_WeaponType);
	m_IsPickedUp = true;
}

bool DroppedItem::IsPickedUp() const
{
	return m_IsPickedUp;
}

void DroppedItem::TryPickUpByPlayer()
{
	if (m_IsPickedUp)
	{
		return;
	}
	const bool isOverlap{ utils::IsOverlapping(m_PickUpHitBox.GetCurrentPoints(),
		m_pPlayer->GetCombatHitBox().GetCurrentPoints())
	};
	if (!isOverlap)
	{
		return;
	}
	GiveToPlayer();
}

void DroppedItem::TryPickUpByEnemy(Enemy* pEnemy)
{
	if (pEnemy->GetState() != Enemy::EnemyState::SearchingWeapon)
	{
		return;
	}
	const bool isOverlap
	{
		utils::IsOverlapping(
			m_PickUpHitBox.GetCurrentPoints(),
			pEnemy->GetBodyHitBox().GetCurrentPoints()
			)
	};
	if (!isOverlap)
	{
		return;
	}
	pEnemy->SetWeaponType(m_WeaponType);
	pEnemy->SetHasDroppedWeapon(false);

	if (pEnemy->CanSeePlayer())
	{
		pEnemy->SetState(Enemy::EnemyState::Walking);
	}
	else
	{
		pEnemy->SetState(Enemy::EnemyState::Patrolling);
	}
	m_IsPickedUp = true;
}

Enemy* DroppedItem::GetOwner() const
{
	return m_pOwner;
}

const HitBox& DroppedItem::GetPickUpHitBox() const
{
	return m_PickUpHitBox;
}

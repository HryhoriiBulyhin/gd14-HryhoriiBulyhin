#pragma once
#include "WorldObject.h"
#include "WeaponType.h"
#include "HitBox.h"
#include "Animation.h"

class Player;
class Enemy;
class ResourceManager;
class Texture;
class DroppedItem final : public WorldObject
{
public:
	explicit DroppedItem(Enemy* pOwner, Player* pPlayer, WeaponType weaponType, ResourceManager* pResources);

	DroppedItem(const DroppedItem& other) = delete;
	DroppedItem& operator=(const DroppedItem& other) = delete;
	DroppedItem(DroppedItem&& other) = delete;
	DroppedItem& operator=(DroppedItem&& other) = delete;

	void Update(float elapsedSec)override;
	void Draw() const override;

	WeaponType GetWeaponType() const;
	bool IsPickedUp()const;

	void TryPickUpByPlayer();
	void TryPickUpByEnemy(Enemy* pEnemy);

	Enemy* GetOwner()const;
	const HitBox& GetPickUpHitBox()const;
private:
	static Vector2f GetDropPosition(Enemy* pOwner);
	void GiveToPlayer();

	Enemy* m_pOwner{};
	Player* m_pPlayer{};

	int m_FrameIndex{},
		m_Columns{};

	WeaponType m_WeaponType{};
	Texture* m_pItemTexture{};
	HitBox m_PickUpHitBox{};

	float m_Scale{};
	float m_DropOffset{};
	bool m_IsPickedUp{};
};

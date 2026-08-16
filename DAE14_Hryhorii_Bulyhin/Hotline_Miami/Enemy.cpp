#include "pch.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "utils.h"
#include "Texture.h"
#include "Player.h"

#include <iostream>

Enemy::Enemy(const Vector2f& position, const JsonImporter::EntityHitboxInfo& hitboxInfo, const JsonImporter::EnemyVisualInfo& visualInfo, ResourceManager* pResources)
	:
	Entity{ EntityType::enemy,position },
	m_WeaponType{ WeaponType::Bat },
	m_State{ EnemyState::Walking },
	m_MoveSpeed{ 300 },
	m_GetUpDealay{ 3.f },
	m_FleeDistance{ 250.f },
	m_FleeSpeed{ 180.f },
	m_PatrolDircetion{ 1.f },
	m_PatrolSpeed{ 150.f },
	m_HasDroppedItem{ false }
{
	LoadAnimationData(m_Dead, visualInfo.dead, nullptr, pResources);

	for (const JsonImporter::VisualPresetInfo& visualPreset : visualInfo.presets)
	{
		WeaponType weaponType{};

		if (!StringToWeaponType(visualPreset.id, weaponType))
		{
			continue;
		}

		const int weaponIndex
		{
			static_cast<int>(weaponType)
		};

		const JsonImporter::HitboxPresetInfo* pHitboxPreset
		{
			FindHitboxPreset(hitboxInfo, visualPreset.id)
		};

		const JsonImporter::AnimationHitboxInfo* pWalkHitboxes{};
		const JsonImporter::AnimationHitboxInfo* pAttackHitboxes{};

		if (pHitboxPreset != nullptr)
		{
			if (pHitboxPreset->hasWalk)
			{
				pWalkHitboxes = &pHitboxPreset->walk;
			}

			if (pHitboxPreset->hasAttack)
			{
				pAttackHitboxes = &pHitboxPreset->attack;
			}
		}
		if (visualPreset.hasWalk)
		{
			LoadAnimationData
			(
				m_Presets[weaponIndex].walk,
				visualPreset.walk,
				pWalkHitboxes,
				pResources
			);
		}

		if (visualPreset.hasAttack)
		{
			LoadAnimationData
			(
				m_Presets[weaponIndex].attack,
				visualPreset.attack,
				pAttackHitboxes,
				pResources
			);
		}
		if (visualPreset.hasDead)
		{
			LoadAnimationData
			(
				m_Presets[weaponIndex].dead,
				visualPreset.dead,
				nullptr,
				pResources
			);
			m_Presets[weaponIndex].hasDead = true;
		}
	}
	ApplyCurrentAnimation();
	RefreshHitbox();
}

void Enemy::Update(float elapsedSec)
{
	UpdateCurrentState(elapsedSec);
	RefreshHitbox();
}

Vector2f Enemy::GetCenter()const
{
	const float frameWidth
	{
		m_pCurrentTexture->GetWidth() / static_cast<float>(m_CurrentAnimation.GetColumns())
	};
	const float frameHeight
	{
		m_pCurrentTexture->GetHeight() / static_cast<float>(m_CurrentAnimation.GetRows())
	};

	return Vector2f
	{
		GetPosition().x + frameWidth * m_Scale / 2.f,
		GetPosition().y + frameHeight * m_Scale / 2.f
	};
}

void Enemy::SetSearchTarget(const Vector2f& targetPosition)
{
	m_SearchTarget = targetPosition;
}

float Enemy::GetRotationAngle() const
{
	return m_RotationAngle;
}

void Enemy::UpdateStunned(float elapsedSec)
{
	UpdateFriction(elapsedSec);

	m_StunnedTime += elapsedSec;

	if (m_StunnedTime >= m_GetUpDealay)
	{
		m_StunnedTime = 0.f;

		if (!m_CanSeePlayer)
		{
			SetState(EnemyState::Patrolling);
		}
		else if (m_WeaponType == WeaponType::Unarmed)
		{
			SetState(EnemyState::Fleeing);
		}
		else
		{
			SetState(EnemyState::Walking);
		}
	}
}

void Enemy::UpdateFleeing(float elapsedSec)
{
	const Vector2f difference{ GetCenter() - m_TargertPlayer->GetCenter() };

	const float distance{ difference.Length() };

	if (distance < m_FleeDistance)
	{
		RotateTowards(GetCenter() + difference);

		MoveDirection(difference, m_FleeSpeed, elapsedSec);
	}
	else
	{
		SetVelocity(Vector2f{});
		m_IsMoving = false;
	}
	UpdateCombat();
}

void Enemy::UpdatePatrolling(float elapsedSec)
{
	if (m_CanSeePlayer)
	{
		SetState(EnemyState::Walking);

		return;
	}
	if (m_Patrol.HasPoints())
	{
		const Vector2f target{ m_Patrol.GetCurrentTarget() };
		const Vector2f difference{ target - GetCenter() };

		const float reachDistance{ 10.f };

		if (difference.Length() <= reachDistance)
		{
			m_Patrol.ReachCurrentTarget();
			return;
		}
		MoveTowards(target, elapsedSec);

		return;
	}
	if (m_HasNavigationTarget)
	{
		MoveTowards(m_NavigationTarget, elapsedSec);

		return;
	}
	// Horizontal movement
	if (m_PatrolDircetion > 0.f)
	{
		m_RotationAngle = 0.f;
	}
	else
	{
		m_RotationAngle = 180.f;
	}
	MoveDirection(Vector2f
		{
			m_PatrolDircetion,
			0.f
		}, m_PatrolSpeed, elapsedSec);
}

void Enemy::UpdateSearchingWeapon(float elapsedSec)
{
	MoveTowards(m_SearchTarget, elapsedSec);
}

void Enemy::UpdateBeingFinished(float elapsedSec)
{
	SetVelocity(Vector2f{});
	m_IsMoving = false;
}

void Enemy::UpdateDead(float elapsedSec)
{
	UpdateFriction(elapsedSec);
	m_CurrentAnimation.Update(elapsedSec);
}


void Enemy::Stun(const Vector2f& hitDir)
{
	SetFallDirection(hitDir);
	SetState(EnemyState::Stunned);
	m_StunnedTime = 0.f;

	m_RotationAngle = GetFallAngle();

	SetVelocity(GetFallDirection() * GetFallForce());
	m_IsMoving = false;
	m_CurrentAnimation.SetStartFrame(0);
}

void Enemy::SetCanSeePlayer(bool canSeePlayer)
{
	m_CanSeePlayer = canSeePlayer;
}

bool Enemy::CanSeePlayer() const
{
	return m_CanSeePlayer;
}

void Enemy::Draw() const
{
	if (m_State == EnemyState::BeingFinished)
	{
		return;
	}

	const float frameWidth
	{
		m_pCurrentTexture->GetWidth() / static_cast<float>(m_CurrentAnimation.GetColumns())
	};

	const float frameHeight
	{
		m_pCurrentTexture->GetHeight() / static_cast<float>(m_CurrentAnimation.GetRows())
	};

	const Rectf srcRect
	{
		m_CurrentAnimation.GetCurrentColumn() * frameWidth,
		m_CurrentAnimation.GetCurrentRow() * frameHeight,
		frameWidth,
		frameHeight
	};

	const float drawWidth
	{
		frameWidth * m_Scale
	};

	const float drawHeight
	{
		frameHeight * m_Scale
	};

	const Vector2f center
	{
		GetPosition().x + drawWidth / 2.f,
		GetPosition().y + drawHeight / 2.f
	};

	const Rectf localDstRect
	{
		-drawWidth / 2.f,
		-drawHeight / 2.f,
		drawWidth,
		drawHeight
	};

	glPushMatrix();
	{
		glTranslatef(center.x, center.y, 0.f);
		glRotatef(m_RotationAngle, 0.f, 0.f, 1.f);

		m_pCurrentTexture->Draw(localDstRect, srcRect);
	}
	glPopMatrix();

	//// Body hitbox
	//utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });

	//utils::DrawPolygon(GetBodyHitBox().GetCurrentPoints(),true,2.f);

	//// Combat hitbox
	//utils::SetColor(Color4f{ 1.f, 1.f, 0.f, 1.f });

	//utils::DrawPolygon(GetCombatHitBox().GetCurrentPoints(),true,2.f);
}

void Enemy::RefreshHitbox()
{
	UpdateBodyHitBox();
	UpdateCombatHitBox();
}

void Enemy::Hit()
{
	const Vector2f hitDirection{ GetCenter() - m_TargertPlayer->GetCenter() };

	SetFallDirection(hitDirection);

	if (m_TargertPlayer->GetWeaponType() == WeaponType::Unarmed)
	{
		Stun(hitDirection);
		return;
	}

	SetState(EnemyState::Dead);
	m_RotationAngle = GetFallAngle();
	SetVelocity(GetFallDirection() * GetFallForce());
	m_IsMoving = false;
}

void Enemy::SetWeaponType(WeaponType weaponType)
{
	m_WeaponType = weaponType;

	ApplyCurrentAnimation();
	RefreshHitbox();
}

void Enemy::SetState(EnemyState state)
{
	const Vector2f previousCenter
	{
		GetCenter()
	};

	m_State = state;
	ApplyCurrentAnimation();

	const float frameWidth
	{
		m_pCurrentTexture->GetWidth() / static_cast<float>(m_CurrentAnimation.GetColumns())
	};

	const float frameHeight
	{
		m_pCurrentTexture->GetHeight() / static_cast<float>(m_CurrentAnimation.GetRows())
	};

	SetPosition(Vector2f
		{
			previousCenter.x - frameWidth * m_Scale / 2.f,
			previousCenter.y - frameHeight * m_Scale / 2.f
		}
	);

	RefreshHitbox();
}

void Enemy::SetTargetPlayer(Player* pPlayer)
{
	m_TargertPlayer = pPlayer;
}

void Enemy::UpdateCombat()
{
	if (!m_CanSeePlayer)
	{
		return;
	}
	const bool isCombatOverlap
	{
		utils::IsOverlapping(
			GetCombatHitBox().GetCurrentPoints(),
			m_TargertPlayer->GetBodyHitBox().GetCurrentPoints()
		)
	};
	if (!isCombatOverlap)
	{
		return;
	}

	// Player attack -> enemy gets damage
	if (m_TargertPlayer->GetState() == Player::PlayerState::Attacking)
	{
		Hit();
		return;
	}
	// Case NEVER attacks player
	if (m_WeaponType == WeaponType::Case)
	{
		return;
	}
	if (m_State == EnemyState::Fleeing)
	{
		return;
	}
	SetVelocity(Vector2f{});
	m_IsMoving = false;
	SetState(EnemyState::Attacking);
}

void Enemy::MoveDirection(const Vector2f& direction, float speed, float elapsedSec)
{
	Vector2f moveDir{ direction };

	if (moveDir.Length() > 0.f)
	{
		moveDir = moveDir.Normalized();
	}
	SetVelocity(moveDir * speed);

	SetPosition(GetPosition() + GetVelocity() * elapsedSec);

	m_IsMoving = true;

	m_CurrentAnimation.Update(elapsedSec);
}

void Enemy::RotateTowards(const Vector2f& target)
{
	const Vector2f direction{ target - GetCenter() };

	m_RotationAngle = std::atan2(direction.y, direction.x) * 180.f / static_cast<float>(M_PI);
}

void Enemy::MoveTowards(const Vector2f& target, float elapsedSec)
{
	RotateTowards(target);

	MoveDirection(target - GetCenter(), m_MoveSpeed, elapsedSec);
}

void Enemy::SetNavigationTarget(const Vector2f& target)
{
	m_NavigationTarget = target;
	m_HasNavigationTarget = true;
}

void Enemy::ClearNavigationTarget()
{
	m_HasNavigationTarget = false;
}

bool Enemy::HasNavigationTarget() const
{
	return m_HasNavigationTarget;
}

void Enemy::SetPatrolPoints(const std::vector<Vector2f>& points)
{
	m_Patrol.SetPoints(points);
}

void Enemy::UpdateBodyHitBox()
{
	const Rectf bodyRect
	{
		-6.f,
		-8.f,
		12.f,
		16.f
	};
	GetBodyHitBox().SetLocalRect(bodyRect);
	GetBodyHitBox().Update(GetCenter(), m_Scale, 0.f);
}
void  Enemy::UpdateCombatHitBox()
{
	const EnemyAnimationData* pAnimationData
	{
		GetCurrentAnimationData()
	};

	const int currentFrame
	{
		m_CurrentAnimation.GetCurrentFrame()
	};

	if (currentFrame < 0 ||
		currentFrame >= static_cast<int>(
			pAnimationData->hitBoxFrames.size())
		)
	{
		return;
	}

	const JsonImporter::HitboxFrameInfo& frameHitBox
	{
		pAnimationData->hitBoxFrames[currentFrame]
	};

	GetCombatHitBox().SetLocalRect(
		Rectf
		{
			frameHitBox.x,
			frameHitBox.y,
			frameHitBox.width,
			frameHitBox.height
		}
	);

	GetCombatHitBox().Update(
		GetCenter(),
		m_Scale,
		m_RotationAngle
	);
}
bool Enemy::HasDroppedItem() const
{
	return m_HasDroppedItem;
}
void Enemy::SetHasDroppedWeapon(bool hasDroppedWeapon)
{
	m_HasDroppedItem = hasDroppedWeapon;
}
void Enemy::LoadAnimationData(EnemyAnimationData& dst, const JsonImporter::AnimationVisualInfo& src, const JsonImporter::AnimationHitboxInfo* pHitboxInfo, ResourceManager* pResources)
{
	dst.rows = src.rows;
	dst.columns = src.cols;
	dst.frameCount = src.frameCount;
	dst.frameTime = src.frameTime;
	dst.scale = src.scale;
	dst.loop = src.loop;

	dst.pTexture = pResources->GetTexture(src.texturePath);

	if (pHitboxInfo != nullptr)
	{
		dst.hitBoxFrames = pHitboxInfo->frames;
	}
}
const JsonImporter::HitboxPresetInfo* Enemy::FindHitboxPreset(const JsonImporter::EntityHitboxInfo& hitboxInfo, const std::string& id) const
{
	for (const JsonImporter::HitboxPresetInfo& preset : hitboxInfo.presets)
	{
		if (preset.id == id)
		{
			return &preset;
		}
	}
	return nullptr;
}
const EnemyAnimationData* Enemy::GetCurrentAnimationData() const
{
	if (m_State == EnemyState::Dead)
	{
		const int weaponIndex{ static_cast<int>(m_WeaponType) };
		const EnemyPresetData& preset{ m_Presets[weaponIndex] };

		if (preset.hasDead)
		{
			return &preset.dead;
		}
		return &m_Dead;
	}
	if (m_State == EnemyState::Stunned || m_State == EnemyState::BeingFinished || m_State == EnemyState::Dead)
	{
		return &m_Dead;
	}

	const int weaponIndex
	{
		static_cast<int>(m_WeaponType)
	};

	const EnemyPresetData& preset
	{
		m_Presets[weaponIndex]
	};

	if (m_State == EnemyState::Attacking &&
		preset.attack.pTexture != nullptr)
	{
		return &preset.attack;
	}

	return &preset.walk;
}

void Enemy::ApplyCurrentAnimation()
{
	const EnemyAnimationData* pAnimationData
	{
		GetCurrentAnimationData()
	};
	m_pCurrentTexture = pAnimationData->pTexture;
	m_Scale = pAnimationData->scale;

	m_CurrentAnimation = Animation
	{
		pAnimationData->rows,
		pAnimationData->columns,
		pAnimationData->frameCount,
		pAnimationData->frameTime,
		pAnimationData->loop
	};

	if (m_State == EnemyState::Walking && m_WeaponType == WeaponType::Unarmed)
	{
		m_CurrentAnimation.SetStartFrame(1);
	}
	else
	{
		m_CurrentAnimation.Reset();
	}
}
void Enemy::ReversePatrolDirection()
{
	m_PatrolDircetion *= -1.f;
}
void Enemy::OnLevelCollision()
{
	if (m_State == EnemyState::Stunned ||
		m_State == EnemyState::Dead)
	{
		SetVelocity(Vector2f{});
		return;
	}
	if (m_State == EnemyState::Patrolling && !m_Patrol.HasPoints())
	{
		ReversePatrolDirection();
	}
}
void Enemy::UpdateCurrentState(float elapsedSec)
{
	switch (m_State)
	{
	case EnemyState::Walking:
	{
		UpdateWalking(elapsedSec);
		break;
	}
	case EnemyState::Attacking:
	{
		UpdateAttacking(elapsedSec);
		break;
	}
	case EnemyState::Stunned:
	{
		UpdateStunned(elapsedSec);
		break;
	}
	case EnemyState::Fleeing:
	{
		UpdateFleeing(elapsedSec);
		break;
	}
	case EnemyState::Patrolling:
	{
		UpdatePatrolling(elapsedSec);
		break;
	}
	case EnemyState::BeingFinished:
	{
		UpdateBeingFinished(elapsedSec);
		break;
	}
	case EnemyState::SearchingWeapon:
	{
		UpdateSearchingWeapon(elapsedSec);
		break;
	}
	case EnemyState::Dead:
	{
		UpdateDead(elapsedSec);
		break;
	}
	}
}
void Enemy::UpdateWalking(float elapsedSec)
{
	if (m_WeaponType == WeaponType::Case)
	{
		SetVelocity(Vector2f{});
		m_IsMoving = false;
		UpdateCombat();

		return;
	}
	if (m_CanSeePlayer)
	{
		MoveTowards(m_TargertPlayer->GetCenter(), elapsedSec);
		UpdateCombat();
	}
	else if (m_HasNavigationTarget)
	{
		MoveTowards(m_NavigationTarget, elapsedSec);
	}
	else
	{
		SetVelocity(Vector2f{});
		m_IsMoving = false;
	}
}
void Enemy::UpdateAttacking(float elapsedSec)
{
	SetVelocity(Vector2f{});
	m_IsMoving = false;

	m_CurrentAnimation.Update(elapsedSec);

	UpdateCombatHitBox();

	if (m_TargertPlayer != nullptr &&
		m_TargertPlayer->GetState() != Player::PlayerState::Dead)
	{
		const bool isHit
		{
			utils::IsOverlapping(
				GetCombatHitBox().GetCurrentPoints(),
				m_TargertPlayer->GetBodyHitBox().GetCurrentPoints()
			)
		};
		if (isHit)
		{
			m_TargertPlayer->SetTargetEnemy(this);
			m_TargertPlayer->Hit();
		}
	}
	if (m_CurrentAnimation.IsFinished())
	{
		SetState(EnemyState::Walking);
	}
}
WeaponType Enemy::GetWeaponType() const
{
	return m_WeaponType;
}

Enemy::EnemyState Enemy::GetState() const
{
	return m_State;
}
#include "pch.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "HitBox.h"
#include "utils.h"
#include "Enemy.h"

#include <iostream>

Player::Player(const Vector2f& position, const JsonImporter::EntityHitboxInfo& hitboxInfo, const JsonImporter::PlayerVisualInfo& visualInfo, ResourceManager* pResources)
	:
	Entity{ EntityType::player,position },
	m_WeaponType{ WeaponType::Unarmed },
	m_State{ PlayerState::Walking }
{
	LoadAnimationData(m_Dead, visualInfo.dead, nullptr, pResources);
	LoadAnimationData(m_Finisher, visualInfo.finisher, nullptr, pResources);

	for (const JsonImporter::VisualPresetInfo& visualPreset : visualInfo.presets)
	{
		WeaponType weaponType{};

		if (!StringToWeaponType(visualPreset.id, weaponType))
		{
			continue;
		}
		const int weaponIndex{ static_cast<int>(weaponType) };
		const JsonImporter::HitboxPresetInfo* pHitboxPreset{ FindHitboxPreset(hitboxInfo,visualPreset.id) };

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
		LoadAnimationData(m_Presets[weaponIndex].walk, visualPreset.walk, pWalkHitboxes, pResources);
		LoadAnimationData(m_Presets[weaponIndex].attack, visualPreset.attack, pAttackHitboxes, pResources);
	}
	ApplyCurrentAnimation();
	UpdateBodyHitbox();
}

void Player::Update(float elapsedSec)
{
	UpdateCurrentState(elapsedSec);
	RefreshHitbox();
}

void Player::UpdateCurrentState(float elapsedSec)
{
	switch (m_State)
	{
	case PlayerState::Walking:
	{
		UpdateWalking(elapsedSec);
		break;
	}
	case PlayerState::Attacking:
	{
		UpdateAttacking(elapsedSec);
		break;
	}
	case PlayerState::Dead:
	{
		UpdateDead(elapsedSec);
		break;
	}
	case PlayerState::Finishing:
	{
		UpdateFinishing(elapsedSec);
		break;
	}
	}
}

void Player::UpdateWalking(float elapsedSec)
{
	UpdateMovement(elapsedSec);

	if (m_WasMoving)
	{
		m_CurrentAnimation.Update(elapsedSec);
	}
	else
	{
		m_CurrentAnimation.Reset();
	}
}

void Player::UpdateAttacking(float elapsedSec)
{
	UpdateMovement(elapsedSec);

	m_CurrentAnimation.Update(elapsedSec);

	if (m_CurrentAnimation.IsFinished())
	{
		SetState(PlayerState::Walking);
	}
}

void Player::UpdateDead(float elapsedSec)
{
	UpdateFriction(elapsedSec);
	m_CurrentAnimation.Update(elapsedSec);
}

void Player::UpdateFinishing(float elapsedSec)
{
	SetVelocity(Vector2f{});
}

void Player::UpdateMovement(float elapsedSec)
{
	SavePreviousPosition();

	const Uint8* pStates{ SDL_GetKeyboardState(nullptr) };

	bool
		moveRight{ static_cast<bool>(pStates[SDL_SCANCODE_D]) },
		moveLeft{ static_cast<bool>(pStates[SDL_SCANCODE_A]) },
		moveDown{ static_cast<bool>(pStates[SDL_SCANCODE_S]) },
		moveUp{ static_cast<bool>(pStates[SDL_SCANCODE_W]) };

	Vector2f moveDirection{};

	if (moveUp)
	{
		moveDirection.y += 1.f;
	}
	if (moveLeft)
	{
		moveDirection.x -= 1.f;
	}
	if (moveDown)
	{
		moveDirection.y -= 1.f;
	}
	if (moveRight)
	{
		moveDirection.x += 1.f;
	}
	m_WasMoving = moveDirection.x != 0.f || moveDirection.y != 0.f;

	if (m_WasMoving)
	{
		moveDirection = moveDirection.Normalized();

		const float moveSpeed{ 350.f };

		SetVelocity(moveDirection * moveSpeed);
		SetPosition(GetPosition() + GetVelocity() * elapsedSec);
	}
	else
	{
		SetVelocity(Vector2f{});
	}
}

void Player::UpdateBodyHitbox()
{
	const Rectf bodyRect
	{
		-6.f,
		-8.f,
		12.f,
		16.f
	};
	GetBodyHitBox().SetLocalRect(bodyRect);
	GetBodyHitBox().Update(GetBodyCenter(), m_Scale, 0.f);
}

void Player::UpdateCombatHitBox()
{
	const PlayerAnimationData* pAnimationData{ GetCurrentAnimationData() };

	if (!pAnimationData->hitBoxFrames.empty())
	{
		const int currentFrame{ m_CurrentAnimation.GetCurrentFrame() };

		if (currentFrame < static_cast<int>(pAnimationData->hitBoxFrames.size()))
		{
			const JsonImporter::HitboxFrameInfo& frameHitBox{ pAnimationData->hitBoxFrames[currentFrame] };

			GetCombatHitBox().SetLocalRect
			(
				Rectf
				{
					frameHitBox.x,
					frameHitBox.y,
					frameHitBox.width,
					frameHitBox.height
				}
			);

			GetCombatHitBox().Update
			(
				GetCenter(),
				m_Scale,
				m_RotationAngle
			);
		}
	}
}

const JsonImporter::HitboxPresetInfo* Player::FindHitboxPreset(const JsonImporter::EntityHitboxInfo& hitboxInfo, const std::string& id) const
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

void Player::Draw() const
{
	const float frameWidth
	{
		m_pCurrentTexture->GetWidth() / static_cast<float>(m_CurrentAnimation.GetColumns())
	};
	const float frameHeight
	{
		m_pCurrentTexture->GetHeight() / static_cast<float>(m_CurrentAnimation.GetRows())
	};
	Rectf srcRect
	{
		m_CurrentAnimation.GetCurrentColumn() * frameWidth,
		m_CurrentAnimation.GetCurrentRow() * frameHeight,
		frameWidth,
		frameHeight
	};
	const Rectf dstRect
	{
		GetPosition().x,
		GetPosition().y,
		frameWidth * m_Scale,
		frameHeight * m_Scale
	};
	const float drawWidth{ frameWidth * m_Scale };
	const float drawHeight{ frameHeight * m_Scale };

	const Vector2f center{ GetPosition().x + drawWidth / 2.f,GetPosition().y + drawHeight / 2.f };
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

	/*utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });

	utils::DrawPolygon
	(
		GetBodyHitBox().GetCurrentPoints(),
		true,
		2.f
	);

	utils::SetColor(Color4f{ 1.f, 1.f, 0.f, 1.f });

	utils::DrawPolygon
	(
		GetCombatHitBox().GetCurrentPoints(),
		true,
		2.f
	);*/
}

void Player::Hit()
{
	if (m_State == PlayerState::Dead ||
		m_State == PlayerState::Finishing ||
		m_pTargetEnemy == nullptr)
	{
		return;
	}
	const Vector2f hitDirection{ GetCenter() - m_pTargetEnemy->GetCenter() };
	SetFallDirection(hitDirection);

	SetState(PlayerState::Dead);

	m_RotationAngle = GetFallAngle();

	SetVelocity(GetFallDirection() * GetFallForce());

	m_WasMoving = false;
}

void Player::SetWeaponType(WeaponType weaponType)
{
	m_WeaponType = weaponType;
	ApplyCurrentAnimation();
	RefreshHitbox();
}

void Player::SetState(PlayerState state)
{
	m_State = state;
	ApplyCurrentAnimation();
	RefreshHitbox();
}

void Player::RefreshHitbox()
{
	UpdateBodyHitbox();
	UpdateCombatHitBox();
}

void Player::SetTargetCrosshair(const Vector2f& targetPosition)
{
	if (m_State == PlayerState::Walking || m_State == PlayerState::Attacking)
	{
		const Vector2f direction{ targetPosition - GetCenter() };
		m_RotationAngle = std::atan2(direction.y, direction.x) * 180.f / static_cast<float>(M_PI);
	}

}

WeaponType Player::GetWeaponType() const
{
	return m_WeaponType;
}

Player::PlayerState Player::GetState() const
{
	return m_State;
}

Vector2f Player::GetCenter() const
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

Vector2f Player::GetBodyCenter() const
{
	//Stable body hitbox
	const int weaponIndex
	{
		static_cast<int>(m_WeaponType)
	};
	const PlayerAnimationData& walk
	{
		m_Presets[weaponIndex].walk
	};
	const float frameWidth
	{
		walk.pTexture->GetWidth() / static_cast<float>(walk.columns)
	};
	const float frameHeight
	{
		walk.pTexture->GetHeight() / static_cast<float>(walk.rows)
	};

	return Vector2f
	{
		GetPosition().x + frameWidth * walk.scale / 2.f,
		GetPosition().y + frameHeight * walk.scale / 2.f
	};
}


void Player::LoadAnimationData(PlayerAnimationData& destination, const JsonImporter::AnimationVisualInfo& source, const JsonImporter::AnimationHitboxInfo* pHitboxInfo, ResourceManager* pResources)
{
	destination.rows = source.rows;
	destination.columns = source.cols;
	destination.frameCount = source.frameCount;
	destination.scale = source.scale;
	destination.loop = source.loop;
	destination.frameTime = source.frameTime;

	destination.pTexture = pResources->GetTexture(source.texturePath);

	if (pHitboxInfo != nullptr)
	{
		destination.hitBoxFrames = pHitboxInfo->frames;
	}
}

void Player::ApplyCurrentAnimation()
{
	const PlayerAnimationData* pAnimationData
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
}
const PlayerAnimationData* Player::GetCurrentAnimationData() const
{
	if (m_State == PlayerState::Dead)
	{
		return &m_Dead;
	}
	if (m_State == PlayerState::Finishing)
	{
		return &m_Finisher;
	}
	const int weaponIndex
	{
		static_cast<int>(m_WeaponType)
	};
	if (m_State == PlayerState::Attacking)
	{
		return &m_Presets[weaponIndex].attack;
	}
	return &m_Presets[weaponIndex].walk;
}
void Player::MouseHandling(const SDL_MouseButtonEvent& e)
{
	if (e.button == SDL_BUTTON_LEFT)
	{
		switch (m_State)
		{
		case PlayerState::Walking:
		{
			StartAttack();
			break;
		}
		case PlayerState::Finishing:
		{
			AdvanceFinisher();
			break;
		}
		case PlayerState::Attacking:
		case PlayerState::Dead:
			break;
		}
	}
}

void Player::KeyboardHandling(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_e:
	{
		m_WantsToPickUp = true;
		break;
	}

	case SDLK_SPACE:
	{
		if (m_State != PlayerState::Dead &&
			m_State != PlayerState::Finishing)
		{
			m_WantsToFinisher = true;
		}
		break;
	}
	}
}

bool Player::WantsToPickUp() const
{
	return m_WantsToPickUp;
}

void Player::ResetPickUpRequest()
{
	m_WantsToPickUp = false;
}

bool Player::CanStartFinisher(Enemy* pEnemy) const
{
	return
		m_State != PlayerState::Dead &&
		m_State != PlayerState::Finishing &&
		pEnemy->GetState() == Enemy::EnemyState::Stunned &&
		utils::IsOverlapping(GetCombatHitBox().GetCurrentPoints(), pEnemy->GetBodyHitBox().GetCurrentPoints());
}

void Player::StartFinisher(Enemy* pEnemy)
{
	m_pFinisherTarget = pEnemy;

	pEnemy->SetState(Enemy::EnemyState::BeingFinished);
	pEnemy->SetVelocity(Vector2f{});

	SetVelocity(Vector2f{});
	m_WasMoving = false;

	m_RotationAngle = pEnemy->GetRotationAngle();

	SetState(PlayerState::Finishing);
	m_CurrentAnimation.SetStartFrame(0);

	const Vector2f enemyCenter
	{
		pEnemy->GetCenter()
	};

	const float frameWidth
	{
		m_pCurrentTexture->GetWidth() /
		static_cast<float>(m_CurrentAnimation.GetColumns())
	};

	const float frameHeight
	{
		m_pCurrentTexture->GetHeight() /
		static_cast<float>(m_CurrentAnimation.GetRows())
	};

	SetPosition(
		Vector2f
		{
			enemyCenter.x - frameWidth * m_Scale / 2.f,
			enemyCenter.y - frameHeight * m_Scale / 2.f
		}
	);

	RefreshHitbox();
}

void Player::StartAttack()
{
	if (m_State == PlayerState::Walking)
	{
		SetState(PlayerState::Attacking);
	}
}
bool Player::WantsToAttack() const
{
	return m_WantsToAttack;
}
void Player::ResetAttackRequest()
{
	m_WantsToAttack = false;
}
void Player::AdvanceFinisher()
{
	if (m_pFinisherTarget == nullptr)
	{
		SetState(PlayerState::Walking);
		return;
	}

	m_CurrentAnimation.AdvanceFrame();
	RefreshHitbox();

	if (!m_CurrentAnimation.IsOnLastFrame())
	{
		return;
	}

	m_pFinisherTarget->SetState(Enemy::EnemyState::Dead);
	m_pFinisherTarget = nullptr;

	SetState(PlayerState::Walking);
}

bool Player::WantsToFinisher() const
{
	return m_WantsToFinisher;
}

void Player::ResetFinisherRequest()
{
	m_WantsToFinisher = false;
}

int Player::GetCurrentAnimationFrame() const
{
	return m_CurrentAnimation.GetCurrentFrame();
}

void Player::SetTargetEnemy(Enemy* pEnemy)
{
	m_pTargetEnemy = pEnemy;
}

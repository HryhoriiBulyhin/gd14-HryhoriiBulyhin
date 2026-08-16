#include "pch.h"
#include "Car.h"
#include "Player.h"
#include "Entity.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "utils.h"

#include <string>
Car::Car(const Vector2f& position, ResourceManager* pResources)
	:
	WorldObject{ Vector2f{ position.x - 100.f,position.y } },
	m_FrameCount{ 13 },
	m_OpenFrame{ 2 },
	m_Scale{ 2.f },
	m_FrameDuration{ 0.06f },
	m_TriggerWidth{ 40.f },
	m_EnterHeight{ 20.f },
	m_State{ CarState::Closed },
	m_ClosedFrame{ 0 },
	m_EngineStartFrame{ 5 },
	m_EngineEndFrame{ 13 }
{
	std::string texturePath{ "Entity/WorldObject/sprPlayerCar_strip13.png" };
	m_pTexture = pResources->GetTexture(texturePath);

	int rows{ 1 }, cols{ 13 };
	m_Animation = Animation
	{
		rows,
		cols,
		m_FrameCount,
		m_FrameDuration,
		false
	};

	m_Animation.SetStartFrame(m_ClosedFrame);
	InitializeHitBoxes();
}

void Car::InitializeHitBoxes()
{
	const float frameWidth{ m_pTexture->GetWidth() / static_cast<float>(m_FrameCount) };
	const float frameHeight{ m_pTexture->GetHeight() };

	const float width{ frameWidth * m_Scale };
	const float height{ frameHeight * m_Scale };

	const Vector2f position{ GetPosition() };

	// This is bad
	m_Bounds = Rectf
	{
		position.x,
		position.y,
		width,
		height
	};
	m_TriggerCollision = Rectf
	{
		m_Bounds.left + m_Bounds.width - 120.f,
		m_Bounds.bottom + 55.f,
		m_TriggerWidth + 20.f,
		m_Bounds.height
	};
	m_EnterCollision = Rectf
	{
		m_Bounds.left + 100.f,
		m_Bounds.bottom + 20.f,
		m_TriggerWidth,
		m_EnterHeight
	};
}

void Car::Draw()const
{
	const float frameWidth{ m_pTexture->GetWidth() / static_cast<float>(m_Animation.GetColumns()) };
	const float frameHeight{ m_pTexture->GetHeight() };

	const Rectf srcRect
	{
		m_Animation.GetCurrentColumn() * frameWidth,
		0.f,
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
	m_pTexture->Draw(dstRect, srcRect);
	//// Debug
	//utils::SetColor(Color4f{ 1.f,0.f,0.f,1.f });
	//utils::DrawRect(m_Bounds, 2.f);
	//
	//utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
	//utils::DrawRect(m_TriggerCollision, 2.f);

	//utils::SetColor(Color4f{ 0.f,1.f,1.f,1.f });
	//utils::DrawRect(m_EnterCollision, 2.f);
}

const Rectf& Car::GetBounds() const
{
	return m_Bounds;
}

bool Car::IsCollidingWithPlayer(const Player& player) const
{
	const std::vector<Vector2f>& points{ player.GetBodyHitBox().GetCurrentPoints() };

	// Full collision
	if (!IsActive())
	{
		return utils::IsOverlapping(
			points,
			m_Bounds
		);
	}

	if (utils::IsOverlapping(points, m_TriggerCollision)
		|| utils::IsOverlapping(points, m_EnterCollision))
	{
		return false;
	}
	return utils::IsOverlapping(points, m_Bounds);
}

void Car::HandlePlayer(Player* pPlayer)
{
	if (!IsActive())
	{
		return;
	}
	const std::vector<Vector2f>& playerHitBox{ pPlayer->GetBodyHitBox().GetCurrentPoints() };

	if (m_HasStartedEngine)
	{
		pPlayer->SetPosition(m_PlayerPos);
		pPlayer->SetVelocity(Vector2f{});
		pPlayer->RefreshHitbox();

		return;
	}
	const bool isInTrigger
	{
		utils::IsOverlapping(
			playerHitBox,
			m_TriggerCollision
		)
	};
	const bool isInEnter
	{
		utils::IsOverlapping(
			playerHitBox,
			m_EnterCollision
		)
	};

	if (isInEnter && m_State == CarState::Open)
	{
		m_HasStartedEngine = true;

		m_PlayerPos = pPlayer->GetPosition();

		pPlayer->SetVelocity(Vector2f{});

		m_State = CarState::Engine;
		m_Animation.SetCurrentFrame(m_EngineStartFrame);

		m_AnimationTime = 0.f;

		return;
	}
	if (isInTrigger && m_State == CarState::Closed)
	{
		m_State = CarState::Opening;
		m_AnimationTime = 0.f;

		return;
	}
	// Out of trigger
	if (!isInTrigger && m_State == CarState::Open)
	{
		m_State = CarState::Closing;
		m_AnimationTime = 0.f;
	}
}

Car::CarState Car::GetState() const
{
	return m_State;
}

bool Car::HasStartedEngine() const
{
	return m_HasStartedEngine;
}

void Car::UpdateOpening(float elapsedSec)
{
	m_AnimationTime += elapsedSec;

	if (m_AnimationTime < m_FrameDuration)
	{
		return;
	}

	m_AnimationTime = 0.f;

	if (m_Animation.GetCurrentFrame() < m_OpenFrame)
	{
		m_Animation.AdvanceFrame();
	}
	if (m_Animation.GetCurrentFrame() >= m_OpenFrame)
	{
		m_Animation.SetCurrentFrame(m_OpenFrame);
		m_State = CarState::Open;
	}
}

void Car::UpdateClosing(float elapsedSec)
{
	m_AnimationTime += elapsedSec;

	if (m_AnimationTime < m_FrameDuration)
	{
		return;
	}

	m_AnimationTime = 0.f;

	if (m_Animation.GetCurrentFrame() > m_ClosedFrame)
	{
		m_Animation.RevereFrame();
	}
	if (m_Animation.GetCurrentFrame() <= m_ClosedFrame)
	{
		m_Animation.SetCurrentFrame(m_ClosedFrame);
		m_State = CarState::Closed;
	}
}

void Car::UpdateEngine(float elapsedSec)
{
	m_AnimationTime += elapsedSec;

	if (m_AnimationTime < m_FrameDuration)
	{
		return;
	}
	m_AnimationTime -= m_FrameDuration;

	if (m_Animation.GetCurrentFrame() < m_EngineEndFrame)
	{
		m_Animation.AdvanceFrame();
	}
	if (m_Animation.GetCurrentFrame() >= m_EngineEndFrame)
	{
		m_Animation.SetCurrentFrame(m_EngineEndFrame);
	}
}

void Car::Update(float elapsedSec)
{
	switch (m_State)
	{
	case CarState::Opening:
	{
		UpdateOpening(elapsedSec);
		break;
	}
	case CarState::Closing:
	{
		UpdateClosing(elapsedSec);
		break;
	}
	case CarState::Engine:
	{
		UpdateEngine(elapsedSec);
		break;
	}
	case CarState::Closed:
	case CarState::Open:
		break;
	}
}

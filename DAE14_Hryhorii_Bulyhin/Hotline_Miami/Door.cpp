#include "pch.h"
#include "Door.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "utils.h"
#include "Player.h"
#include "Enemy.h"

Door::Door(const JsonImporter::DoorInfo& doorInfo, ResourceManager* pResources)
	:
	WorldObject{ doorInfo.position },
	m_PushForce{ 300.f },
	m_FrictionPerSecond{ 7.f },
	m_StopVelocity{ 40.f },
	m_MaxAngle{ 150.f },
	m_SolidAngle{ 35.f },
	m_HitVelocity{ 80.f },
	m_LocalRect{ doorInfo.rect }
{
	Load(doorInfo);
	LoadTextures(pResources);
	InitializeVertices(doorInfo);
	UpdateVertices();
}
bool Door::IsSolid() const
{
	return std::abs(m_Angle) < m_SolidAngle;
}
const Vector2f* Door::GetCurrentPoints() const
{
	return m_WorldVertices;
}
int Door::GetVertexCount() const
{
	return m_VertexCount;
}
float Door::GetAngle() const
{
	return m_Angle;
}
float Door::GetAngularVelocity() const
{
	return m_AngularVelocity;
}
bool Door::WasPushedByPlayer() const
{
	return m_WasPushedByPlayer;
}
void Door::Load(const JsonImporter::DoorInfo& doorInfo)
{
	if (doorInfo.subType == "Horizontal")
	{
		m_Orientation = Orientation::Horizontal;
	}
	else if (doorInfo.subType == "Vertical")
	{
		m_Orientation = Orientation::Vertical;
	}
}

void Door::LoadTextures(ResourceManager* pResources)
{
	if (m_Orientation == Orientation::Horizontal)
	{
		std::string texturePath{ "Entity/WorldObject/sprDoorH2.png" };
		m_pTexture = pResources->GetTexture(texturePath);
	}
	else
	{
		std::string texturePath{ "Entity/WorldObject/sprDoorV.png" };
		m_pTexture = pResources->GetTexture(texturePath);
	}
}

void Door::InitializeVertices(const JsonImporter::DoorInfo& doorInfo)
{
	const Rectf& rect{ doorInfo.rect };

	if (m_Orientation == Orientation::Horizontal)
	{
		m_LocalVertices[0] = Vector2f{ 0.f,-rect.height / 2.f };
		m_LocalVertices[1] = Vector2f{ rect.width,-rect.height / 2.f };
		m_LocalVertices[2] = Vector2f{ rect.width,rect.height / 2.f };
		m_LocalVertices[3] = Vector2f{ 0.f,rect.height / 2.f };
	}
	else
	{
		m_LocalVertices[0] = Vector2f{ -rect.width / 2.f,0.f };
		m_LocalVertices[1] = Vector2f{ rect.width / 2.f,0.f };
		m_LocalVertices[2] = Vector2f{ rect.width / 2.f,rect.height };
		m_LocalVertices[3] = Vector2f{ -rect.width / 2.f, rect.height };
	}
}

void Door::ClampAngle()
{
	if (m_Angle > m_MaxAngle)
	{
		m_Angle = m_MaxAngle;
		m_AngularVelocity = 0.f;
	}
	else if (m_Angle < -m_MaxAngle)
	{
		m_Angle = -m_MaxAngle;
		m_AngularVelocity = 0.f;
	}
}

void Door::UpdateVertices()
{
	const Vector2f pivot{ GetPivot() };

	const float radians{ m_Angle * static_cast<float>(M_PI) / 180.f };

	const float cosA{ std::cos(radians) };
	const float sinA{ std::sin(radians) };

	for (int i = 0; i < m_VertexCount; ++i)
	{
		const Vector2f& local{ m_LocalVertices[i] };

		m_WorldVertices[i].x = pivot.x + local.x * cosA -
			local.y * sinA;
		m_WorldVertices[i].y = pivot.y + local.x * sinA +
			local.y * cosA;
	}
}

void Door::ApplyFriction(float elapsedSec)
{
	float friction{ 1.f - m_FrictionPerSecond * elapsedSec };

	if (friction < 0.f)
	{
		friction = 0.f;
	}

	m_AngularVelocity *= friction;
}

bool Door::IsOverlappingEntity(const Entity& entity) const
{
	const std::vector<Vector2f>& entityPoints
	{
		entity.GetBodyHitBox().GetCurrentPoints()
	};

	for (const Vector2f& point : entityPoints)
	{
		if (utils::IsPointInPolygon(point, m_WorldVertices, m_VertexCount))
		{
			return true;
		}
	}

	for (int i = 0; i < m_VertexCount; ++i)
	{
		if (utils::IsPointInPolygon(m_WorldVertices[i], entityPoints))
		{
			return true;
		}
	}
	return false;
}

Vector2f Door::GetPivot() const
{
	if (m_Orientation == Orientation::Horizontal)
	{
		return Vector2f
		{
			GetPosition().x + m_LocalRect.left,
			GetPosition().y + m_LocalRect.bottom + m_LocalRect.height / 2.f
		};
	}
	return Vector2f
	{
		GetPosition().x + m_LocalRect.left + m_LocalRect.width / 2.f,
		GetPosition().y + m_LocalRect.bottom
	};
}

void Door::Update(float elapsedSec)
{
	m_Angle += m_AngularVelocity * elapsedSec;

	ApplyFriction(elapsedSec);

	if (std::abs(m_AngularVelocity) < m_StopVelocity)
	{
		m_AngularVelocity = 0.f;
		m_WasPushedByPlayer = false;
	}
	ClampAngle();
	UpdateVertices();
}

void Door::Draw() const
{
	const Vector2f pivot{ GetPivot() };

	Rectf drawRect{};

	if (m_Orientation == Orientation::Horizontal)
	{
		drawRect = Rectf
		{
			0.f,
			-m_LocalRect.height / 2.f,
			m_LocalRect.width,
			m_LocalRect.height
		};
	}
	else
	{
		drawRect = Rectf
		{
			-m_LocalRect.width / 2.f,
			0.f,
			m_LocalRect.width,
			m_LocalRect.height
		};
	}

	glPushMatrix();
	{
		glTranslatef(pivot.x, pivot.y, 0.f);
		glRotatef(m_Angle, 0.f, 0.f, 1.f);

		m_pTexture->Draw(drawRect);
	}
	glPopMatrix();

	/*utils::SetColor(Color4f{1.f, 1.f, 0.f, 1.f});
	utils::DrawPolygon(m_WorldVertices, m_VertexCount, true, 2.f);

	utils::SetColor(Color4f{ 1.f, 0.f, 1.f, 1.f });
	utils::FillEllipse(pivot, 4.f, 4.f);*/
}

void Door::Push(const Vector2f& pusherPos)
{
	const Vector2f pivot{ GetPivot() };
	const Vector2f pivotToPusher{ pusherPos - pivot };
	const float radians{ m_Angle * static_cast<float>(M_PI) / 180.f };

	Vector2f doorDirection{};

	if (m_Orientation == Orientation::Horizontal)
	{
		doorDirection = Vector2f
		{
			std::cos(radians),
			std::sin(radians)
		};
	}
	else
	{
		doorDirection = Vector2f
		{
			-std::sin(radians),
			std::cos(radians)
		};
	}

	const float cross{ doorDirection.CrossProduct(pivotToPusher) };

	if (cross > 0.f)
	{
		m_AngularVelocity = -m_PushForce;
	}
	else
	{
		m_AngularVelocity = m_PushForce;
	}
}

void Door::HandlePlayer(Player* pPlayer, const Vector2f& moveDir)
{
	if (moveDir.Length() > 0.f && IsOverlappingEntity(*pPlayer))
	{
		m_WasPushedByPlayer = true;
		Push(pPlayer->GetCenter());
	}
}

void Door::HandleEnemy(Enemy* pEnemy, const Vector2f& moveDir)
{
	if (pEnemy->GetState() == Enemy::EnemyState::Dead)
	{
		return;
	}
	if (!IsOverlappingEntity(*pEnemy))
	{
		return;
	}
	if (m_WasPushedByPlayer && std::abs(m_AngularVelocity) >= m_HitVelocity)
	{
		const Vector2f hitDir{ pEnemy->GetCenter() - GetPivot() };
		pEnemy->Stun(hitDir);
	}
	// Enemy pushes the door
	else if (moveDir.Length() > 0.f)
	{
		m_WasPushedByPlayer = false;
		Push(pEnemy->GetCenter());
	}
}

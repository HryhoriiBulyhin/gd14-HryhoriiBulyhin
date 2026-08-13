#include "pch.h"
#include "HitBox.h"

HitBox::HitBox(const Rectf& localRect)
	:
	m_LocalRect{ localRect }
{
	const int m_AmountOfPoints{ 4 };

	m_CurrentPoints.reserve(m_AmountOfPoints);
	m_PreviousPoints.reserve(m_AmountOfPoints);
}

void HitBox::SetLocalRect(const Rectf& localRect)
{
	m_LocalRect = localRect;
}

void HitBox::Update(const Vector2f& center, float scale, float rotationAngle)
{
	const int m_AmountOfPoints{ 4 };

	m_PreviousPoints = m_CurrentPoints;
	m_CurrentPoints.clear();

	const float left{ m_LocalRect.left * scale };
	const float right{ (m_LocalRect.left + m_LocalRect.width) * scale };
	const float bottom{ m_LocalRect.bottom * scale };
	const float top{ (m_LocalRect.bottom + m_LocalRect.height) * scale };

	const Vector2f localPoints[m_AmountOfPoints]
	{
		Vector2f{left,bottom},
		Vector2f{right,bottom},
		Vector2f{right,top},
		Vector2f{left,top}
	};

	for (const Vector2f& localPoint : localPoints)
	{
		const Vector2f rotatedPoint
		{
			RoatatePoint(localPoint,rotationAngle)
		};

		m_CurrentPoints.push_back(Vector2f
			{
				center.x + rotatedPoint.x,
				center.y + rotatedPoint.y
			}
		);
	}
}

const std::vector<Vector2f>& HitBox::GetCurrentPoints() const
{
	return m_CurrentPoints;
}

Vector2f HitBox::RoatatePoint(const Vector2f& point, float angle) const
{
	const float radians{ angle * static_cast<float>(M_PI) / 180.f };
	const float cosine{ std::cos(radians) };
	const float sine{ std::sin(radians) };

	return Vector2f
	{
		point.x * cosine - point.y * sine,
		point.x * sine + point.y * cosine
	};
}

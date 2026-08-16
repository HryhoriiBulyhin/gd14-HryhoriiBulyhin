#include "pch.h"
#include "EnemyPatrol.h"

EnemyPatrol::EnemyPatrol(const std::vector<Vector2f>& points)
	:
	m_Points{ points },
	m_Direction{ 1 }
{
}

void EnemyPatrol::SetPoints(const std::vector<Vector2f>& points)
{
	m_Points = points;
	m_CurrentIndex = 0;
	m_Direction = 1;
}

const Vector2f& EnemyPatrol::GetCurrentTarget() const
{
	return m_Points[m_CurrentIndex];
}

void EnemyPatrol::ReachCurrentTarget()
{
	if (m_Points.size() <= 1)
	{
		return;
	}
	if (m_Direction > 0)
	{
		if (m_CurrentIndex >= m_Points.size() - 1)
		{
			m_Direction = -1;
			--m_CurrentIndex;
		}
		else
		{
			++m_CurrentIndex;
		}
	}
	else
	{
		if (m_CurrentIndex == 0)
		{
			m_Direction = 1;
			++m_CurrentIndex;
		}
		else
		{
			--m_CurrentIndex;
		}
	}
}

bool EnemyPatrol::HasPoints() const
{
	return !m_Points.empty();
}

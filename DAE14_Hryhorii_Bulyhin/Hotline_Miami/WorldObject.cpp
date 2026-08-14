#include "pch.h"
#include "WorldObject.h"
WorldObject::WorldObject(const Vector2f& position)
	:
	m_IsActive{ true },
	m_Position{ position }
{
}
void WorldObject::SetPosition(const Vector2f& position)
{
	m_Position = position;
}

const Vector2f& WorldObject::GetPosition() const
{
	return m_Position;
}

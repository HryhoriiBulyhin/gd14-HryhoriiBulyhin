#include "pch.h"
#include "Camera.h"

Camera::Camera(float viewPortWidth, float viewPortHeight)
	:
	m_ViewPortWidth{ viewPortWidth },
	m_ViewPortHeight{ viewPortHeight },
	m_Zoom{ 1.25f }
{
}
void Camera::Follow(const Vector2f& targertPosition)
{
	m_Position.x = targertPosition.x - m_ViewPortWidth / 2.f;
	m_Position.y = targertPosition.y - m_ViewPortHeight / 2.f;
}
void Camera::BeginDraw() const
{
	const float cameraCenterX
	{
		m_Position.x + m_ViewPortWidth / 2.f
	};

	const float cameraCenterY
	{
		m_Position.y + m_ViewPortHeight / 2.f
	};
	glPushMatrix();
	{
		glTranslatef(m_ViewPortWidth / 2.f, m_ViewPortHeight / 2.f, 0.f);
		glScalef(m_Zoom, m_Zoom, 1.f);
		glTranslatef(-cameraCenterX, -cameraCenterY, 0.f);
	}
}
void Camera::EndDraw() const
{
	glPopMatrix();
}
const Vector2f& Camera::GetPosition() const
{
	return m_Position;
}

float Camera::GetZoom() const
{
	return m_Zoom;
}

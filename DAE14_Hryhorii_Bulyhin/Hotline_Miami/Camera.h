#pragma once
class Camera final
{
public:
	explicit Camera(float viewPortWidth, float viewPortHeight);
	void Follow(const Vector2f& targertPosition);

	void BeginDraw() const;
	void EndDraw() const;
	const Vector2f& GetPosition() const;
	float GetZoom()const;
private:
	Vector2f m_Position{};
	float m_ViewPortWidth{};
	float m_ViewPortHeight{};
	float m_Zoom{};
};


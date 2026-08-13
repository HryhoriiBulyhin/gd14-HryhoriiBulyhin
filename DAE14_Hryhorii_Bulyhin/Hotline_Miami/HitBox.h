#pragma once
#include <vector>
class HitBox final
{
public:
	HitBox() = default;
	explicit HitBox(const Rectf& localRect);

	void SetLocalRect(const Rectf& localRect);
	void Update(const Vector2f& center, float scale, float rotationAngle);

	const std::vector<Vector2f>& GetCurrentPoints() const;
private:

	Vector2f RoatatePoint(const Vector2f& point, float angle)const;
	Rectf m_LocalRect{};
	std::vector<Vector2f> m_CurrentPoints{};
	std::vector<Vector2f> m_PreviousPoints{};
};


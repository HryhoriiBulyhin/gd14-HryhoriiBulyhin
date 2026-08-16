#pragma once
#include <vector>

class EnemyPatrol final
{
public:
	EnemyPatrol() = default;
	explicit EnemyPatrol(const std::vector<Vector2f>& points);

	void SetPoints(const std::vector<Vector2f>& points);

	const Vector2f& GetCurrentTarget() const;

	void ReachCurrentTarget();

	bool HasPoints() const;

private:
	std::vector<Vector2f> m_Points{};

	size_t m_CurrentIndex{};
	int m_Direction{};
};


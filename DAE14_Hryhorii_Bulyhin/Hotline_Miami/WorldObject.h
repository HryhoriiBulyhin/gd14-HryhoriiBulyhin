#pragma once
class WorldObject
{
public:
	explicit WorldObject(const Vector2f& position);
	virtual ~WorldObject() = default;

	virtual void Update(float elapsedSec) = 0;
	virtual void Draw()const = 0;

	void SetPosition(const Vector2f& position);
	const Vector2f& GetPosition() const;

	void SetActive(bool isActive);
	bool IsActive()const;
private:
	Vector2f m_Position{};
	bool m_IsActive{};
};


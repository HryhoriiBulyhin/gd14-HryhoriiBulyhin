#pragma once
#include "WeaponType.h"

class Texture;
class ResourceManager;

class Corpse final
{
public:
	struct CorpseData final
	{
		Vector2f position{};
		float rotationAngle{};
		WeaponType weaponType{};
	};

	explicit Corpse(const CorpseData& data, ResourceManager* pResources);

	void Draw() const;

	const CorpseData& GetData() const;

private:
	CorpseData m_Data{};

	Texture* m_pTexture{};

	int m_Cols{};
	int m_FrameIndex{};

	float m_Scale{};
};


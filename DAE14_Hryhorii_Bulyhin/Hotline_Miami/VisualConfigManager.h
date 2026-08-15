#pragma once
#include "JsonImporter.h"

#include <string>
#include <vector>

class VisualConfigManager final
{
public:
	VisualConfigManager() = default;

	bool Load(std::string jsonPath);

	const JsonImporter::PlayerVisualInfo& GetPlayerVisualInfo() const;
	const JsonImporter::EnemyVisualInfo& GetEnemyVisualInfo() const;

	const JsonImporter::VisualPresetInfo* GetPlayerPreset(const std::string& id) const;
	const JsonImporter::VisualPresetInfo* GetEnemyPreset(const std::string& id) const;
	const JsonImporter::AnimationVisualInfo& GetMouseVisualInfo()const;
private:
	const JsonImporter::VisualPresetInfo* FindPreset(const std::vector<JsonImporter::VisualPresetInfo>& presets, const std::string& id)const;

	JsonImporter::VisualConfigData m_VisualConfig{};
	bool m_IsLoaded{};
};


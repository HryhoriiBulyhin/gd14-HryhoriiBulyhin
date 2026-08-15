#include "pch.h"
#include "VisualConfigManager.h"

#include <iostream>
bool VisualConfigManager::Load(std::string jsonPath)
{
	m_VisualConfig = JsonImporter::ImportVisualConfig(jsonPath);

	m_IsLoaded = !m_VisualConfig.player.presets.empty() || !m_VisualConfig.enemy.presets.empty();

	if (!m_IsLoaded)
	{
		std::cout << "Could not load visual config: "
			<< jsonPath
			<< '\n';

		return false;
	}
	return true;
}

const JsonImporter::PlayerVisualInfo& VisualConfigManager::GetPlayerVisualInfo() const
{
	return m_VisualConfig.player;
}

const JsonImporter::EnemyVisualInfo& VisualConfigManager::GetEnemyVisualInfo() const
{
	return m_VisualConfig.enemy;
}

const JsonImporter::VisualPresetInfo* VisualConfigManager::GetPlayerPreset(const std::string& id) const
{
	return FindPreset(m_VisualConfig.player.presets, id);
}

const JsonImporter::VisualPresetInfo* VisualConfigManager::GetEnemyPreset(const std::string& id) const
{
	return FindPreset(m_VisualConfig.enemy.presets, id);
}

const JsonImporter::AnimationVisualInfo& VisualConfigManager::GetMouseVisualInfo() const
{
	return m_VisualConfig.mouse;
}

const JsonImporter::VisualPresetInfo* VisualConfigManager::FindPreset(const std::vector<JsonImporter::VisualPresetInfo>& presets, const std::string& id) const
{
	for (size_t presetIndex = 0; presetIndex < presets.size(); ++presetIndex)
	{
		const JsonImporter::VisualPresetInfo& preset{ presets.at(presetIndex) };

		if (preset.id == id)
		{
			return &preset;
		}
	}
	return nullptr;
}


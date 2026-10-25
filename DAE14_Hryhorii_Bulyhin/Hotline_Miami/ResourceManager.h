#pragma once
#include <vector>
#include <memory>

class Texture;

struct TextureResource final
{
	explicit TextureResource(const std::string& path, std::unique_ptr<Texture> pTexture);

	// I don't need anymore unique_ptr prevents copies

	/*TextureResource(const TextureResource& other) = delete;
	TextureResource& operator=(const TextureResource& other) = delete;*/

	// Compiler generates it for me

	/*TextureResource(TextureResource&& other) noexcept = default;
	TextureResource& operator=(TextureResource&& other) noexcept = default;*/

	std::string path{};
	std::unique_ptr<Texture> m_pTexture{};
};
class ResourceManager final
{
public:
	ResourceManager() = default;

	// I don't need anymore unique_ptr prevents copies

	//ResourceManager(const ResourceManager& other) = delete;
	//ResourceManager& operator=(const ResourceManager& other) = delete;

	// Compiler generates it for me

	/*ResourceManager(ResourceManager&& other) noexcept = default;
	ResourceManager& operator=(ResourceManager&& other) noexcept = default;*/

	Texture* GetTexture(const std::string& path);

	void Clear();

private:
	std::vector<TextureResource> m_Textures{};
};


#pragma once
#include <vector>
class Texture;

struct TextureResource
{
	std::string path;
	Texture* pTexture;

	TextureResource(const std::string& path, Texture* pTexture);

	TextureResource(TextureResource&& other);
	TextureResource& operator=(TextureResource&& other);

	TextureResource(const TextureResource& other) = delete;
	TextureResource& operator=(const TextureResource& other) = delete;
};
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;

	ResourceManager(ResourceManager&& other);
	ResourceManager& operator=(ResourceManager&& other);

	Texture* GetTexture(const std::string& path);
	void Clear();

private:
	std::vector<TextureResource> m_Textures;
};


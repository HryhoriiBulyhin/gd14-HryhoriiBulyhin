#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"

#include <iostream>

TextureResource::TextureResource(const std::string& path, std::unique_ptr<Texture> pTexture)
	:
	path{ path },
	m_pTexture{std::move(pTexture)}
{
}

Texture* ResourceManager::GetTexture(const std::string& path)
{
	for (TextureResource& resource : m_Textures)
	{
		if (resource.path == path)
		{
			return resource.m_pTexture.get();
		}
	}
	auto texture{ std::make_unique<Texture>(path) };
	m_Textures.push_back(TextureResource{ path,std::move(texture)});

	return m_Textures.back().m_pTexture.get();
}

void ResourceManager::Clear()
{
	m_Textures.clear();
}

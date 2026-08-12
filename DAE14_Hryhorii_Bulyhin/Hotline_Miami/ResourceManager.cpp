#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"

#include <iostream>
ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	Clear();
}

ResourceManager::ResourceManager(ResourceManager&& other)
	:
	m_Textures{ std::move(other.m_Textures) }
{
}

ResourceManager& ResourceManager::operator=(ResourceManager&& other)
{
	if (this != &other)
	{
		Clear();

		m_Textures = std::move(other.m_Textures);
	}
	return *this;
}

Texture* ResourceManager::GetTexture(const std::string& path)
{
	for (TextureResource& resource : m_Textures)
	{
		if (resource.path == path)
		{
			return resource.pTexture;
		}
	}

	Texture* pTexture = new Texture{ path };
	m_Textures.push_back(TextureResource{ path,pTexture });

	return pTexture;
}

void ResourceManager::Clear()
{
	for (TextureResource& resource : m_Textures)
	{
		delete resource.pTexture;
		resource.pTexture = nullptr;
	}
	m_Textures.clear();
}

TextureResource::TextureResource(const std::string& path, Texture* pTexture)
	:
	path{ path },
	pTexture{ pTexture }
{
}

TextureResource::TextureResource(TextureResource&& other)
	:
	path{ std::move(other.path) },
	pTexture{ other.pTexture }
{
	other.pTexture = nullptr;
}

TextureResource& TextureResource::operator=(TextureResource&& other)
{
	if (this != &other)
	{
		delete pTexture;

		path = std::move(other.path);
		pTexture = other.pTexture;

		other.pTexture = nullptr;
	}
	return *this;
}
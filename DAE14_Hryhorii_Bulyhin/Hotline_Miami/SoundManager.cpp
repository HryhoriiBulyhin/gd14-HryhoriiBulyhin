#include "pch.h"
#include "SoundManager.h"
#include "SoundEffect.h"
#include "SoundStream.h"

#include <iostream>

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
	for (SoundEffectInfo& effect : m_Effects)
	{
		delete effect.pEffect;
		effect.pEffect = nullptr;
	}

	for (SoundStreamInfo& stream : m_Streams)
	{
		delete stream.pStream;
		stream.pStream = nullptr;
	}
}

void SoundManager::AddEffect(const std::string& name, const std::string& path)
{

	for (const SoundEffectInfo& effect : m_Effects)
	{
		if (effect.name == name)
		{
			return;
		}
	}
	SoundEffect* pEffect = new SoundEffect{ path };

	if (name == "door_open")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "door_hit")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "pickup_weapon")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "swing1" || name == "swing2")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "cut1" || name == "cut2")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "punch")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "door_hit")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "hit")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "car_open")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "car_close")
	{
		pEffect->SetVolume(30);
	}
	else if (name == "car_engine")
	{
		pEffect->SetVolume(30);
	}

	m_Effects.push_back(SoundEffectInfo{ name, pEffect });
}

void SoundManager::PlayEffect(const std::string& name, int loops)
{
	for (SoundEffectInfo& effect : m_Effects)
	{
		if (effect.name == name)
		{
			effect.pEffect->Play(loops);
			return;
		}
	}
	std::cout << "Effect not found" << name << '\n';
}

void SoundManager::AddStream(const std::string& name, const std::string& path)
{
	SoundStream* pStream = new SoundStream{ path };
	if (name == "music_prologue")
	{
		pStream->SetVolume(20);
	}
	m_Streams.push_back(SoundStreamInfo{ name,pStream });
}

void SoundManager::PlayStream(const std::string& name, bool repeat)
{
	for (SoundStreamInfo& stream : m_Streams)
	{
		if (stream.name == name)
		{
			stream.pStream->Play(repeat);
			return;
		}
	}
	std::cout << "Stream not found" << name << '\n';
}

void SoundManager::StopAllEffects()
{
	SoundEffect::StopAll();
}

void SoundManager::StopStream()
{
	SoundStream::Stop();
}

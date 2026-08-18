#pragma once
#include <vector>

class SoundEffect;
class SoundStream;

struct SoundEffectInfo
{
	std::string name;
	SoundEffect* pEffect;
};

struct SoundStreamInfo
{
	std::string name;
	SoundStream* pStream;
};
class SoundManager final
{
public:
	SoundManager();
	~SoundManager();

	SoundManager(const SoundManager& other) = delete;
	SoundManager& operator=(const SoundManager& other) = delete;

	void AddEffect(const std::string& name, const std::string& path);
	void PlayEffect(const std::string& name, int loops = 0);

	void AddStream(const std::string& name, const std::string& path);
	void PlayStream(const std::string& name, bool repeat = true);

	void StopAllEffects();
	void StopStream();

private:
	std::vector <SoundEffectInfo> m_Effects;
	std::vector<SoundStreamInfo> m_Streams;
};
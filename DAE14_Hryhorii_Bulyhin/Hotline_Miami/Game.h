#pragma once
#include "BaseGame.h"
#include "JsonImporter.h"
#include "VisualConfigManager.h"

class Texture;
class LevelPositioner;
class Level;
class Player;
class ResourceManager;
class Camera;
class Mouse;
class Enemy;
class DroppedItem;
class LevelManager;
class GameSoundController;
class Game : public BaseGame
{
public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update(float elapsedSec) override;
	void Draw() const override;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override;
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override;
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override;
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override;
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:

	// FUNCTIONS
	void Initialize();
	void Cleanup();
	void ClearBackground() const;

	VisualConfigManager m_VisualConfigManager{};
	JsonImporter::GameData m_LevelData{};
	ResourceManager* m_pResourceManager{};

	Camera* m_pCamera{};
	Mouse* m_pMouse{};
	std::vector<Enemy*> m_Enemies;

	LevelManager* m_pLevelManager{};
	GameSoundController* m_pGameSoundContoller{};
#pragma region FPS
	float m_FPSAccumulatedTime{};
	int m_FPSFrames{};
	int m_CurrentAvarageFPS{};
	void FPS(float elapsedSec);
#pragma endregion FPS
};
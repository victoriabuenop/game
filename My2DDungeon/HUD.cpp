#include "HUD.h"

using namespace DungeonGame;

void HUD::InitializeHudSprites(SDL_Renderer* pRenderer, const std::string& crownFilepath, const std::string& healthFilepath, const std::string& gameOverFilepath, const std::string& victoryFilepath)
{
	CrownTexture = LoadTexture(pRenderer, crownFilepath);
	HealthTexture = LoadTexture(pRenderer, healthFilepath);
	GameOverTexture = LoadTexture(pRenderer, gameOverFilepath);
	VictoryTexture = LoadTexture(pRenderer, victoryFilepath);
}

void HUD::Render(SDL_Renderer* pRenderer, const PlayerState& playerState, const WorldState& worldState, const Vector2d& baseTranslation)
{
	// Health display
	for (unsigned int i = 0; i < playerState.PlayerHP; i++)
	{
		MyTexture = HealthTexture;
		if (MyTexture != nullptr)
		{
			SDL_Rect destRect = {
				(int)(672.0f - 8.0f - 48.0f * i),
				(int)8.0f,
				(int)Size.X,
				(int)Size.Y };
			SDL_RenderCopy(pRenderer, MyTexture, nullptr, &destRect);
		}
	}

	// Inventory display
	for (unsigned int i = 0; i < playerState.ItemInventory.size(); i++)
	{
		ItemType currItem = playerState.ItemInventory[i];
		MyTexture = CrownTexture;
		if (MyTexture != nullptr)
		{
			SDL_Rect destRect = {
				(int)(8.0f + 48.0f * i),
				(int)8.0f,
				(int)Size.X,
				(int)Size.Y };
			SDL_RenderCopy(pRenderer, MyTexture, nullptr, &destRect);
		}
	}

	if (playerState.PlayerHP <= 0)
	{
		
		SDL_Rect destRect = {
				0,
				0,
				WINDOW_WIDTH,
				WINDOW_HEIGHT };
		SDL_RenderCopy(pRenderer, GameOverTexture, nullptr, &destRect);
	}

	if (playerState.PlayerHasWon())
	{
		SDL_Rect destRect = {
				0,
				0,
				WINDOW_WIDTH,
				WINDOW_HEIGHT };
		SDL_RenderCopy(pRenderer, VictoryTexture, nullptr, &destRect);
	}

}
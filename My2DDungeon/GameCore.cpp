#include "GameCore.h"
#include "Sprite.h"
#include "Hero.h"
#include "RoomRenderer.h"
#include "Item.h"
#include "Foe.h"
#include "HUD.h"
#include "BG.h"

namespace DungeonGame
{
	std::vector<Sprite*> SpriteList;

	bool PlayerState::PlayerHasWon() const
	{
		int numberOfCrowns = 0;
		for (int i = 0; i < ItemInventory.size(); i++)
		{
			if (ItemInventory[i] == Item_CROWN)
			{
				numberOfCrowns++;
			}
		}

		return numberOfCrowns >= 3;
	}

	bool WorldState::IsWalkableTile(const Vector2d& inPosition)
	{
		int column = (int)(inPosition.X / TileSizeInPixels.X);
		int row = (int)(inPosition.Y / TileSizeInPixels.Y);
		
		char currentTile = ' ';
		int index = row * TilesPerRow + column;
		if (index >= 0 && index < (int)Tiles.size())
		{
			currentTile = Tiles[index];
		}

		return currentTile == '.';
	}


	void InitializeGame(SDL_Renderer* pRenderer, PlayerState& playerState, WorldState& worldState)
	{
		playerState.HasFinishedGame = false;
		playerState.PlayerHP = 3;
		playerState.InvincibilitySeconds = 0.0f;

		playerState.WantsToGoUp = false;
		playerState.WantsToGoDown = false;
		playerState.WantsToGoLeft = false;
		playerState.WantsToGoRight = false;

		worldState.SecondsSincePlayerDefeat = 0.0f;

		worldState.TilesPerRow = 19;
		worldState.TileSizeInPixels = Vector2d(54.0f, 54.0f);
		worldState.Tiles =
			"  #####            "
			" #....############ "
			"#..............#.##"
			"#..##.....###...#.#"
			"#.........#...#...#"
			"###...#####...##..#"
			"#.......#.........#"
			"#..###....##......#"
			"#..#...#.....###..#"
			" ################# ";

		worldState.Foes.push_back({ true, Vector2d(2.0f * worldState.TileSizeInPixels.X, 5.0f * worldState.TileSizeInPixels.Y) });
		worldState.Foes.push_back({ true, Vector2d(17.0f * worldState.TileSizeInPixels.X, 8.0f * worldState.TileSizeInPixels.Y) });
		worldState.Items.push_back({ true, Item_CROWN, Vector2d(2.0f * worldState.TileSizeInPixels.X, 9.0f * worldState.TileSizeInPixels.Y) });
		worldState.Items.push_back({ true, Item_CROWN, Vector2d(16.0f * worldState.TileSizeInPixels.X, 8.0f * worldState.TileSizeInPixels.Y) });
		worldState.Items.push_back({ true, Item_CROWN, Vector2d(15.0f * worldState.TileSizeInPixels.X, 3.0f * worldState.TileSizeInPixels.Y) });

		BG* bg = new BG;
		bg->Initialize(pRenderer, "Assets/background.bmp");
		bg->Size = Vector2d(WINDOW_WIDTH, WINDOW_HEIGHT);
		SpriteList.push_back(bg);



		RoomRenderer* roomRenderer = new RoomRenderer;
		roomRenderer->InitializeRoomTiles(pRenderer, "Assets/floor00.bmp", "Assets/wall00.bmp");
		SpriteList.push_back(roomRenderer);

		for (unsigned int i = 0; i < worldState.Items.size(); i++)
		{
			std::string itemFilepath = "Assets/crown00.bmp";
			Item* newItem = new Item;
			newItem->Initialize(pRenderer, itemFilepath);
			newItem->SetItemIndex(i);
			SpriteList.push_back(newItem);
		}

		for (unsigned int i = 0; i < worldState.Foes.size(); i++)
		{
			std::string foeFilepath = "Assets/ghost00.bmp";
			Foe* newFoe = new Foe;
			newFoe->Initialize(pRenderer, foeFilepath);
			newFoe->SetFoeIndex(i);
			SpriteList.push_back(newFoe);
		}

		Hero* newHero = new Hero;
		newHero -> Initialize(pRenderer, "Assets/hero00.bmp");
		playerState.PlayerPosition = Vector2d(3.0f * worldState.TileSizeInPixels.X, 2.0f * worldState.TileSizeInPixels.Y);
		//newHero -> Position = Vector2d(100.0f, 120.0f);
		SpriteList.push_back(newHero);

		HUD* newHUD = new HUD;
		newHUD->InitializeHudSprites(pRenderer, "Assets/crown00.bmp", "Assets/health00.bmp", "Assets/gameover.bmp", "Assets/victory.bmp");
		SpriteList.push_back(newHUD);	

	}
	void GetInput(PlayerState& playerState, const WorldState& worldState)
	{
		SDL_Event e = {};
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				playerState.HasFinishedGame = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				auto keyCode = e.key.keysym.sym;
				switch (keyCode)
				{
				case SDLK_ESCAPE:
					playerState.HasFinishedGame = true;
					break;

				case SDLK_w:
				case SDLK_UP:
					playerState.WantsToGoUp = true;
					break;

				case SDLK_s:
				case SDLK_DOWN:
					playerState.WantsToGoDown = true;
					break;

				case SDLK_a:
				case SDLK_LEFT:
					playerState.WantsToGoLeft = true;
					break;

				case SDLK_d:
				case SDLK_RIGHT:
					playerState.WantsToGoRight = true;
					break;
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				auto keyCode = e.key.keysym.sym;
				switch (keyCode)
				{
				case SDLK_w:
				case SDLK_UP:
					playerState.WantsToGoUp = false;
					break;

				case SDLK_s:
				case SDLK_DOWN:
					playerState.WantsToGoDown = false;
					break;

				case SDLK_a:
				case SDLK_LEFT:
					playerState.WantsToGoLeft = false;
					break;

				case SDLK_d:
				case SDLK_RIGHT:
					playerState.WantsToGoRight = false;
					break;
				}

			}
		}
	}
	void UpdateGame(float deltaSeconds, PlayerState& playerState, WorldState& worldState)
	{

		for (unsigned int i = 0; i < SpriteList.size(); i++)
		{
			SpriteList[i]->Update(deltaSeconds, playerState, worldState);
		}

		if (playerState.PlayerHP <= 0 || playerState.PlayerHasWon())
		{
			worldState.SecondsSincePlayerDefeat += deltaSeconds;
			if (worldState.SecondsSincePlayerDefeat > 2.5f)
			{
				playerState.HasFinishedGame = true;
			}
		}
	}
	void RenderGame(SDL_Renderer* pRenderer, const PlayerState& playerState, const WorldState& worldState)
	{
		SDL_RenderClear(pRenderer);

		for (unsigned int i = 0; i < SpriteList.size(); i++)
		{
			SpriteList[i]->Render(pRenderer, playerState, worldState, Vector2d(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f) - worldState.CameraPosition);
		}


		SDL_RenderPresent(pRenderer);
	}
	void CleanupGame(PlayerState& playerState, WorldState& worldState)
	{
		for (unsigned int i = 0; i < SpriteList.size(); i++)
		{
			SpriteList[i]->Cleanup();
			delete SpriteList[i];
		}
		SpriteList.clear();
		Sprite::CleanupTextures();
	}
}


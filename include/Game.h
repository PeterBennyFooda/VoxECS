#pragma once
#include "game_client/GameScene.h"

namespace VoxelWasteland 
{
	class Game
	{
	public:
		Game();
		~Game();

		void Run();

	private:
		void Init();
		void Update();
		void Render();
		void PollEvents();

		sf::RenderWindow* window;
		bool running = false;

		float lastFrameTime = 0.0f;
		float frameTime = 0.0f;
		sf::Clock clock;

		GameScene* scene;
	};
}

#pragma once
#include "core/world/ChunkManager.h"

namespace VoxelWasteland
{
	class GameScene;

	class World
	{
	public:
		World(sf::RenderWindow* win, GameScene* scene);
		~World();

		void Init();
		void Update(float delta);

	private:
		World() = default;

		GameScene* scene;
		sf::RenderWindow* window;

		std::vector<Core::Chunk*> chunks;
		Core::ChunkManager* chunkManager;
	};
}
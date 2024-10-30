#include "game_client/system/World.h"
#include "game_client/GameScene.h"

namespace VoxelWasteland
{
	World::World(sf::RenderWindow* win, GameScene* scene) : 
		window(win), scene(scene), chunkManager(nullptr)
	{
		Init();
	}

	World::~World()
	{
		delete chunkManager;
	}

	/*
	* @brief Initialize world
	*/
	void World::Init()
	{
		// create the world entity
		Entity world = scene->CreateEntity<CTransform, CMap>();
		scene->SetWorld(world);

		// create chunks
		auto& entities = scene->GetEntities();

		CTransform* transform = scene->GetComponent<CTransform>(world);
		CMap* map = scene->GetComponent<CMap>(world);
		
		// generate world around player
		Entity player = scene->GetPlayer();
		CTransform* playerTransform = scene->GetComponent<CTransform>(player);

		if (transform && map && playerTransform)
		{
			sf::Clock clock;
			Core::ChunkManager* chunkManager = new Core::ChunkManager(false, true);
			chunkManager->GenerateChunks(chunks, playerTransform->position);
			std::cout << "Time to generate chunks: " << clock.getElapsedTime().asSeconds() << "s" << std::endl;

			for (auto& chunk : chunks)
			{
				Entity c = scene->CreateEntity<CTransform, CRenderable, CChunk>();

				CTransform* ct = scene->GetComponent<CTransform>(c);
				float posX = chunk->GetOffset().x + (CHUNK_SIZE * (BLOCK_SIZE * 2.0f) * 0.5f);
				float posY = CHUNK_HEIGHT * (BLOCK_SIZE * 2.0f) * 0.5f;
				float posZ = chunk->GetOffset().y + (CHUNK_SIZE * (BLOCK_SIZE * 2.0f) * 0.5f);
				glm::vec3 pos = glm::vec3(posX, posY, posZ);
				ct->position = pos;

				CChunk* cc = scene->GetComponent<CChunk>(c);
				cc->chunk = chunk;
				cc->active = true;

				map->chunkEntities.push_back(c);
			}
		}
	}

	/*
	* @brief Update world
	* @param delta: Delta time
	*/
	void World::Update(float delta)
	{
		for (auto& chunk : chunks)
		{
			chunk->Update();
		}
	}
}
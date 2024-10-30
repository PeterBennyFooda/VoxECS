#include "game_client/GameScene.h"

namespace VoxelWasteland
{
	GameScene::GameScene() : 
		entityCount(0), window(nullptr), 
		controls(nullptr), renderer(nullptr), 
		weapons(nullptr), physics(nullptr), 
		world(nullptr), worldEntity(0)
	{
		entities.reserve(MAX_ENTITIES);
	}

	GameScene::~GameScene()
	{
		delete controls;
		delete renderer;
	}

	/*
	* @brief Create an entity without any components
	* @return The entity
	*/
	Entity GameScene::CreateEntity()
	{
		if (entityCount >= MAX_ENTITIES)
			throw std::runtime_error("Failed to create entity. Maximum number of entities reached.");

		entities.emplace_back(entityCount++);
		return entities.back();
	}

	/*
	* @brief Delete an entity from the scene
	* @param entity: The entity to delete
	* @return True if the entity was deleted, false otherwise
	*/
	bool GameScene::DestroyEntity(const Entity& entity)
	{
		if (std::find(entities.begin(), entities.end(), entity) != entities.end())
		{
			transformPool.RemoveComponent(entity);
			rectTransformPool.RemoveComponent(entity);
			renderablePool.RemoveComponent(entity);
			cameraPool.RemoveComponent(entity);
			physicsPool.RemoveComponent(entity);
			characterPool.RemoveComponent(entity);
			controllablePool.RemoveComponent(entity);
			weaponPool.RemoveComponent(entity);
			mapPool.RemoveComponent(entity);

			entities.erase(entities.begin() + entity);
			entityCount--;

			return true;
		}

		return false;
	}

	/*
	* @brief Get all entities in the scene
	* @return A vector of entities
	*/
	const std::vector<Entity>& GameScene::GetEntities() const
	{
		return entities;
	}

	/*
	* @brief Set the world entity
	* @param e: The world entity
	*/
	void GameScene::SetWorld(Entity e)
	{
		worldEntity = e;
	}

	/*
	* @brief Get the world entity
	* @return The world entity
	*/
	Entity GameScene::GetWorld() const
	{
		return worldEntity;
	}

	/*
	* @brief Set the player entity
	* @param e: The player entity
	*/
	void GameScene::SetPlayer(Entity e)
	{
		playerEntity = e;
	}

	/*
	* @brief Get the player entity
	* @return The player entity
	*/
	Entity GameScene::GetPlayer() const
	{
		return playerEntity;
	}

	/*
	* @brief Initialize the game scene
	*/
	void GameScene::Init(sf::RenderWindow* win)
	{
		window = win;

		controls = new Controls(window, this);
		renderer = new Renderer(window, this);
		camera = new Camera(window, this);
		physics  = new Physics(window, this);
		world = new World(window, this);
		weapons = new Weapons(window, this);
		hud = new HUD(window, this);
	}

	/*
	* @brief Update the game scene
	* @param delta: Time since the last update
	*/
	void GameScene::Update(float delta)
	{
		if(controls)
			controls->Update(delta);

		if (renderer)
			renderer->Update(delta);

		if (camera)
			camera->Update(delta);

		if (world)
			world->Update(delta);

		if (weapons)
			weapons->Update(delta);

		if (physics)
			physics->Update(delta);

		if (hud)
			hud->Update(delta);
	}
	
	/*
	* @brief Render the game
	*/
	void GameScene::Render()
	{
		if (renderer)
			renderer->Render();
	}
}
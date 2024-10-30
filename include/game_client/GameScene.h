#pragma once
#include "core/ECS/Component.h"
#include "GameComponents.h"

#include "game_client/system/Controls.h"
#include "game_client/system/Renderer.h"
#include "game_client/system/Camera.h"
#include "game_client/system/World.h"
#include "game_client/system/Physics.h"
#include "game_client/system/Weapons.h"
#include "game_client/system/HUD.h"

namespace VoxelWasteland
{
	using Entity = Core::ECS::Entity;
	using CTransformPool = Core::ECS::ComponentPool<CTransform>;
	using CRectTransformPool = Core::ECS::ComponentPool<CRectTransform>;
	using CRenderablePool = Core::ECS::ComponentPool<CRenderable>;
	using CCameraPool = Core::ECS::ComponentPool<CCamera>;
	using CPhysicsPool = Core::ECS::ComponentPool<CPhysics>;
	using CCharacterPool = Core::ECS::ComponentPool<CCharacter>;
	using CControllablePool = Core::ECS::ComponentPool<CControllable>;
	using CMapPool = Core::ECS::ComponentPool<CMap>;
	using CChunkPool = Core::ECS::ComponentPool<CChunk>;
	using CWeaponPool = Core::ECS::ComponentPool<CWeapon>;

	class GameScene final
	{
	public:
		GameScene();
		~GameScene();
		
		template <typename... Components>
		Entity CreateEntity();
		Entity CreateEntity();
		bool DestroyEntity(const Entity& entity);
		const std::vector<Entity>& GetEntities() const;

		template<typename Compoenent>
		void AddComponentToEntity(const Entity& entity);
		template<typename Compoenent>
		Compoenent* GetComponent(const Entity& entity);

		void SetWorld(Entity e);	
		Entity GetWorld() const;

		void SetPlayer(Entity e);
		Entity GetPlayer() const;

		void Init(sf::RenderWindow* win);
		void Update(float delta);
		void Render();

	private:
		/*
		*	EEEEE
		*	E
		*	EEEEE
		*	E
		*	EEEEE
		*/
		// ===== Entities =====
		std::vector<Entity> entities;
		size_t entityCount;


		/*
		*	 CCCCC
		*	C
		*	C
		*	C
		 *	 CCCCC
		*/
		// ===== Component Pools =====
		CTransformPool transformPool;
		CRectTransformPool rectTransformPool;
		CRenderablePool renderablePool;
		CCameraPool cameraPool;
		CPhysicsPool physicsPool;
		CCharacterPool characterPool;
		CControllablePool controllablePool;
		CWeaponPool weaponPool;
		CMapPool mapPool;
		CChunkPool chunkPool;

		/*
		*	SSSSS
		*   S
		*   SSSSS
		*	    S
		*	SSSSS
		*/
		// ===== Systems =====
		Controls* controls;
		Renderer* renderer;
		Camera* camera;
		World* world;
		Physics* physics;
		Weapons* weapons;
		HUD* hud;

		// ===== Other Properties =====
		sf::RenderWindow* window;
		Entity worldEntity;
		Entity playerEntity;
	};
}
#include "GameScene.inl"
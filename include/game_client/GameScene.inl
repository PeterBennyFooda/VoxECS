#include "GameScene.h"

namespace VoxelWasteland
{
	/*
	* @brief Create an entity with the given components
	* @return The entity
	*/
	template<typename ...Components>
	inline Entity GameScene::CreateEntity()
	{
		if (entityCount >= MAX_ENTITIES)
			throw std::runtime_error("Failed to create entity. Maximum number of entities reached.");

		Entity entity = entityCount++;

		//C++17 fold expression to call AddComponentToEntity for each component
		(AddComponentToEntity<Components>(entity), ...);

		entities.emplace_back(entity);
		return entity;
	}

	/*
	* @brief Helper function to add a component to an entity
	* @param entity: The entity to add the component to
	*/
	template<typename Compoenent>
	inline void GameScene::AddComponentToEntity(const Entity& entity)
	{
		if constexpr (std::is_same<Compoenent, CTransform>::value)
		{
			if (transformPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CRectTransform>::value)
		{
			if (rectTransformPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CRenderable>::value)
		{
			if (renderablePool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CCamera>::value)
		{
			if (cameraPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CPhysics>::value)
		{
			if (physicsPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CCharacter>::value)
		{
			if (characterPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CControllable>::value)
		{
			if (controllablePool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CWeapon>::value)
		{
			if (weaponPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CMap>::value)
		{
			if (mapPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else if constexpr (std::is_same<Compoenent, CChunk>::value)
		{
			if (chunkPool.AddComponent(entity) == nullptr)
			{
				throw std::runtime_error("Failed to add component to entity. Pool is full.");
			}
		}
		else
		{
			throw std::runtime_error("Unknown component type.");
		}
	}

	/*
	* @brief Get a component from an entity
	* @param entity: The entity to get the component from
	* @return The component
	*/
	template<typename Compoenent>
	inline Compoenent* GameScene::GetComponent(const Entity& entity)
	{
		if constexpr (std::is_same<Compoenent, CTransform>::value)
		{
			return transformPool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CRectTransform>::value)
		{
			return rectTransformPool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CRenderable>::value)
		{
			return renderablePool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CCamera>::value)
		{
			return cameraPool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CPhysics>::value)
		{
			return physicsPool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CCharacter>::value)
		{
			return characterPool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CControllable>::value)
		{
			return controllablePool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CWeapon>::value)
		{
			return weaponPool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CChunk>::value)
		{
			return chunkPool.GetComponent(entity);
		}
		else if constexpr (std::is_same<Compoenent, CMap>::value)
		{
			return mapPool.GetComponent(entity);
		}

		return nullptr;
	}
}
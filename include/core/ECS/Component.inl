#include "Component.h"

namespace VoxelWasteland::Core::ECS
{
	template<typename T>
	inline ComponentPool<T>::ComponentPool() : count(0), capacity(MAX_ENTITIES)
	{
		components.reserve(capacity);
		sparseSet = Utility::SparseSet(capacity, capacity);
	}

	template <typename T>
	inline ComponentPool<T>::ComponentPool(size_t cap) : count(0)
	{
		components.reserve(cap);
		sparseSet = Utility::SparseSet(cap, cap);
		capacity = cap;
	}

	template <typename T>
	inline ComponentPool<T>::~ComponentPool()
	{
		for (T* component : components)
		{
			delete component;
		}
		components.clear();
	}

	/*
	* @brief Add a component to the component pool
	* @param entity: entity to add the component to
	* @return pointer to the component
	*/
	template<typename T>
	inline T* ComponentPool<T>::AddComponent(const Entity& entity)
	{
		if (count < capacity)
		{
			if (sparseSet.Insert(entity))
			{
				T* component = new T();
				components.emplace_back(component);
				count++;

				return component;
			}	
		}

		return nullptr;
	}

	/*
	* @brief Get a component from the component pool
	* @param entity: entity to get the component from
	* @return pointer to the component
	*/
	template<typename T>
	inline T* ComponentPool<T>::GetComponent(const Entity& entity)
	{
		int index = sparseSet.Search(entity);
		if (index != -1) 
		{
			return components[index];
		}

		return nullptr;
	}

	/*
	* @brief Remove a component from the component pool
	* @param entity: entity to remove the component from
	*/
	template<typename T>
	inline bool ComponentPool<T>::RemoveComponent(const Entity& entity)
	{
		int index = sparseSet.Search(entity);
		if (index != -1)
		{
			// Q: why don't we use vector::erase here?
			// A: SparseSet::Delete replaces the element to delete with 
			// the last element in the dense array, not shifting the elements
			if (sparseSet.Delete(entity))
			{
				T* targetComponent = components[index];
				T* lastComponent = components.back();

				components[index] = lastComponent;
				components.pop_back();
				delete targetComponent;

				count--;
				return true;
			}

		}

		return false;
	}
}
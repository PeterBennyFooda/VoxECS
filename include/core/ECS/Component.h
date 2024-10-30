#pragma once
#include "Entity.h"

namespace VoxelWasteland::Core::ECS
{
	template <typename T>
	class ComponentPool
	{
	public:
		ComponentPool();
		ComponentPool(size_t cap);
		~ComponentPool();

		T* AddComponent(const Entity& entity);
		T* GetComponent(const Entity& entity);

		bool RemoveComponent(const Entity& entity);

	private:
		size_t count;
		size_t capacity;
		Utility::SparseSet sparseSet = Utility::SparseSet(10, 10);
		std::vector<T*> components;
	};
}
#include "Component.inl"
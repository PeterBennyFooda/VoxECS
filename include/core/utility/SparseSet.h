#pragma once

namespace VoxelWasteland::Core::Utility
{
	class SparseSet
	{
	public:
		SparseSet(size_t maxVal, size_t cap);
		~SparseSet();

		bool Insert(size_t val);
		bool Delete(size_t val);

		int Search(size_t val) const;
		int Get(size_t index) const;
		SparseSet* Intersect(const SparseSet& other) const;

		size_t Count() const;
		void Clear();

		void Print() const;

	private:
		SparseSet() = default; // not allowed to be called

		size_t* sparse;
		size_t* dense;

		size_t maxValue;
		size_t capacity;
		size_t count;
	};
}
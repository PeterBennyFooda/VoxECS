#include "core/utility/SparseSet.h"

namespace VoxelWasteland::Core::Utility
{
	/*
	* @brief Constructor
	* @param maxVal: maximum value that can be stored in the dense array, which is also the size of the sparse set
	* @param capacity: capacity of the dense array
	*/
	SparseSet::SparseSet(size_t maxVal, size_t cap)
	{
		sparse = new size_t[maxVal+1];
		dense = new size_t[cap];
		maxValue = maxVal;
		capacity = cap;

		count = 0;
	}

	SparseSet::~SparseSet()
	{
		// delete array if size is not 0
		if (count > 0)
		{
			delete[] sparse;
			delete[] dense;
		}
	}

	/*
	* @brief Insert a value into the sparse set
	* @param val: value to insert
	* @return true if the value was inserted, false otherwise
	*/
	bool SparseSet::Insert(size_t val)
	{
		// if the value is greater than the maximum value, return false
		if(val > maxValue)
			return false;

		// if the dense array is full, return false
		if (count >= capacity)
			return false;

		// if the value is already in the sparse set, return false
		if(Search(val) != -1)
			return false;

		sparse[val] = count;	// val as the key to the index in the dense array
		dense[count] = val;		// val as the n-th element in the dense array
		count++;				// increment the number of elements in the dense array

		return true;
	}

	/*
	* @brief Delete a value from the sparse set
	* @param val: value to delete
	* @return true if the value was deleted, false otherwise
	*/
	bool SparseSet::Delete(size_t val)
	{
		// if the value is not in the sparse set, return false
		if (Search(val) == -1)
			return false;

		size_t target_index = sparse[val];
		size_t tmp = dense[count - 1];		// get the last element in the dense array
		dense[target_index] = tmp;			// replace the element to delete with the last element
		sparse[tmp] = target_index;			// update the index of the last element in the sparse array

		count--;							// decrement the number of elements in the dense array

		return true;
	}

	/*
	* @brief Search for a value in the sparse set
	* @param val: value to search for
	* @return the index of the value in the dense array, -1 if the value is not found
	*/
	int SparseSet::Search(size_t val) const
	{
		// if the value is greater than the maximum value, return false
		if (val > maxValue)
			return -1;

		// 1. check if the index returned by the sparse array is less than count (the number of elements in the dense array)
		// 2. check if the value at the index returned by the sparse array is equal to the value we are searching for
		if (sparse[val] < count && dense[sparse[val]] == val)
			return sparse[val];

		// not found
		return -1;
	}

	/*
	* @brief Get a value from the dense array
	* @param index: index of the value in the dense array
	* @return the value at the index in the dense array, -1 if the index is out of bounds
	*/
	int SparseSet::Get(size_t index) const
	{
		// check if the index is within the bounds of the dense array
		if (index < count)
			return dense[index];

		return -1;
	}

	/*
	* @brief Check if the sparse set intersects with another sparse set
	* @param other: the other sparse set to check intersection with
	* @return a new sparse set that contains the intersection of the two sparse sets
	*/
	SparseSet* SparseSet::Intersect(const SparseSet& other) const
	{
		size_t result_cap = std::min(capacity, other.capacity);
		size_t result_max = std::max(maxValue, other.maxValue);

		SparseSet* result = new SparseSet(result_max, result_cap);

		if (count < other.count) // if current sparse set has less elements than the other sparse set
		{
			for (int i = 0; i < count; ++i)
			{
				if (other.Search(dense[i]) != -1)
					result->Insert(dense[i]);
			}
		}
		else // if current sparse set has more elements than the other sparse set
		{
			for (int i = 0; i < other.count; ++i)
			{
				if (Search(other.dense[i]) != -1)
					result->Insert(other.dense[i]);
			}
		}

		return result;
	}

	/*
	*  @brief Get the number of elements in the dense array
	*  @return the number of elements in the dense array
	*/
	size_t SparseSet::Count() const
	{
		return count;
	}

	/*
	* @brief Clear all values from the sparse set
	*/
	void SparseSet::Clear()
	{
		count = 0;
	}

	/*
	* @brief Print all values in the dense array
	*/
	void SparseSet::Print() const
	{
		std::cout << "Dense array: ";
		for (int i = 0; i < count; i++)
			std::cout << dense[i] << " ";
		std::cout << std::endl;
	}
}
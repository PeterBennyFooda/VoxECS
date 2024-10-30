#include "core/world/Block.h"

namespace VoxelWasteland::Core 
{
	Block::Block(BlockData data, bool active) :
		data(data), active(active)
	{
	}

	Block::~Block()
	{
	}

	/*
	* @brief set the active state of the block
	* @param active: the active state of the block
	*/
	void Block::SetActive(bool active)
	{
		this->active = active;
	}

	/*
	* @brief get the active state of the block
	* @return the active state of the block
	*/
	bool Block::IsActive() const
	{
		return active;
	}

	/*
	* @brief set the data of the block
	* @param BlockData& data: the data of the block
	*/
	void Block::SetData(const BlockData& data)
	{
		this->data = data;
	}

	/*
	* @brief get the data of the block
	* @return the data of the block
	*/
	BlockData& Block::GetData()
	{
		return data;
	}
	const BlockData& Block::GetData() const
	{
		return data;
	}
}


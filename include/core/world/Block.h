#pragma once

namespace VoxelWasteland::Core 
{
	class Block
	{
	public:
		Block() = delete;
		Block(BlockData data, bool active = true);
		~Block();

		void SetActive(bool active);
		bool IsActive() const;

		void SetData(const BlockData& data);
		BlockData& GetData();
		const BlockData& GetData() const;

	private:
		BlockData data;
		bool active;
	};
}

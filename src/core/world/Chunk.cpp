#include "core/world/Chunk.h"
#include "core/render/CustomShader.h"
#include "core/ResourceHelper.h"
#include "core/utility/CommonUtils.h"

namespace VoxelWasteland::Core
{
	Chunk::Chunk() : mesh(nullptr), offset(0.0f, 0.0f)
	{
		blocks = std::vector<Block>(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE, Block(BlockData(), false));
	}

	Chunk::~Chunk()
	{
		delete mesh;
	}

	/*
	* @brief update the chunk
	*/
	void Chunk::Update()
	{
		if (dirty)
		{
			UpdateMesh();
			dirty = false;
		}
	}

	/*
	* @brief draw the chunk
	* @param shader: the shader to use to draw the chunk
	*/
	void Chunk::Draw(CustomShader* shader)
	{
		if (mesh)
		{
			mesh->Draw(shader);
		}
	}

	// 1. x * CHUNK_SIZE * CHUNK_SIZE gets which slice of the 3D array we are in
	// 2. y * CHUNK_SIZE gets which row of the slice we are in
	// 3. z gets which element of the row we are in
	Block& Chunk::operator()(const int& x, const int& y, const int& z)
	{
		return blocks[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z];
	}
	const Block& Chunk::operator()(const int& x, const int& y, const int& z) const
	{
		return blocks[x * CHUNK_SIZE * CHUNK_HEIGHT + y * CHUNK_SIZE + z];
	}

	/*
	* @brief try to add a block adjacent to the specified position
	* @param pos: the position of the block
	* @param id: the id of the block
	* @return true if the block was added, false otherwise
	*/
	bool Chunk::TryAddBlock(const glm::vec3& pos, const unsigned int id)
	{
		glm::vec3 blockPos;

		if (CheckBlockExists(pos.x, pos.y, pos.z, blockPos))
		{
			//std::cout << "==start==" << std::endl;
			//std::cout << "clicked block pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

			glm::vec3 diff = pos - blockPos;
			glm::vec3 direction = glm::normalize(diff) * BLOCK_SIZE * 2.0f;

			if (diff == glm::vec3(0))
				direction = glm::vec3(0);

			// Get the absolute values of the components
			float absX = std::abs(direction.x);
			float absY = std::abs(direction.y);
			float absZ = std::abs(direction.z);

			glm::vec3 targetBlockPos = blockPos;

			// Compare the components to find the dominant axis
			if (absX >= absY && absX >= absZ) {
				targetBlockPos.x += direction.x;
			}
			else if (absY >= absX && absY >= absZ) {
				targetBlockPos.y += direction.y;
			}
			else {
				targetBlockPos.z += direction.z;
			}
			
			return AddBlock(targetBlockPos, id);
			//return CreateTree(targetBlockPos);
		}

		return false;
	}

	/*
	* @brief try to add multiple blocks adjacent to the specified positions
	* @param hit: the position of the hit
	* @param positions: the positions of the blocks
	* @param ids: the id of the blocks
	* @return true if the blocks were added, false otherwise
	*/
	bool Chunk::TryAddBlocks(const glm::vec3& hit, const std::vector<glm::vec3>& positions, const std::vector<unsigned int>& ids)
	{
		glm::vec3 blockPos;

		if (CheckBlockExists(hit.x, hit.y, hit.z, blockPos))
		{
			glm::vec3 diff = hit - blockPos;
			glm::vec3 direction = glm::normalize(diff) * BLOCK_SIZE * 2.0f;

			if (diff == glm::vec3(0))
				direction = glm::vec3(0);

			// Get the absolute values of the components
			float absX = std::abs(direction.x);
			float absY = std::abs(direction.y);
			float absZ = std::abs(direction.z);

			glm::vec3 targetBlockPos = blockPos;

			// Compare the components to find the dominant axis
			if (absX >= absY && absX >= absZ) {
				targetBlockPos.x += direction.x;
			}
			else if (absY >= absX && absY >= absZ) {
				targetBlockPos.y += direction.y;
			}
			else {
				targetBlockPos.z += direction.z;
			}

			glm::vec3 offset = targetBlockPos - blockPos;
			std::vector<glm::vec3> newPositions;
			for (auto& p : positions)
			{
				newPositions.push_back(p + offset);
			}
			
			return AddBlocks(newPositions, ids);
		}

		return false;
	}

	/*
	* @brief add a block to the chunk directly at the specified position
	* @param pos: the position of the block
	* @param id: the id of the block
	* @return true if the block was added, false otherwise
	*/
	bool Chunk::AddBlock(const glm::vec3& pos, const unsigned int id)
	{
		const std::unordered_map<int, BlockData>& blockData = ResourceHelper::GetBlockData();
		bool outOfBounds = false;
		glm::vec3 newBlockPos = pos;
		
		//std::cout << "pre New block pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		if (CheckBlockExists(pos.x, pos.y, pos.z, newBlockPos, outOfBounds))
			return false;
		//std::cout << "New block pos: " << newBlockPos.x << ", " << newBlockPos.y << ", " << newBlockPos.z << std::endl;
		//std::cout << "==end==" << std::endl;

		glm::vec3 localIndex = GetBlockIndex(newBlockPos.x, newBlockPos.y, newBlockPos.z);

		int indexX = localIndex.x;
		int indexY = localIndex.y;
		int indexZ = localIndex.z;

		if (outOfBounds)
		{
			if (indexX < 0 && nebighours.left)
				return nebighours.left->AddBlock(pos, id);
			else if (indexX >= CHUNK_SIZE && nebighours.right)
				return nebighours.right->AddBlock(pos, id);
			else if (indexZ < 0 && nebighours.back)
				return nebighours.back->AddBlock(pos, id);
			else if (indexZ >= CHUNK_SIZE && nebighours.front)
				return nebighours.front->AddBlock(pos, id);
		}
		else
		{
			Block& newBlock = GetBlockActual(newBlockPos.x, newBlockPos.y, newBlockPos.z);
			newBlock.SetActive(true);
			newBlock.SetData(blockData.at(id));

			// Update the neighbours for AO
			if (indexX == 0 && nebighours.left)
				nebighours.left->dirty = true;
			if (indexX == CHUNK_SIZE - 1 && nebighours.right)
				nebighours.right->dirty = true;
			if (indexZ == 0 && nebighours.back)
				nebighours.back->dirty = true;
			if (indexZ == CHUNK_SIZE - 1 && nebighours.front)
				nebighours.front->dirty = true;

			dirty = true;
			return true;
		}

		return false;
	}

	/*
	* @brief add multiple blocks to the chunk
	* @param pos: the positions of the blocks
	* @param ids: the ids of the blocks
	* @return true if the blocks were added, false otherwise
	*/
	bool Chunk::AddBlocks(const std::vector<glm::vec3>& positions, const const std::vector<unsigned int> ids)
	{
		if (positions.size() != ids.size())
		{
			std::cout << "Posistions size does not match ids size!" << std::endl;
			return false;
		}

		bool success = false;
		const std::unordered_map<int, BlockData>& blockData = ResourceHelper::GetBlockData();
		
		for (int i = 0; i<positions.size(); ++i)
		{
			const glm::vec3& pos = positions[i];
			const unsigned int id = ids[i];

			bool outOfBounds = false;
			glm::vec3 newBlockPos = pos;

			//std::cout << "pre New block pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
			if (CheckBlockExists(pos.x, pos.y, pos.z, newBlockPos, outOfBounds))
				continue;
			//std::cout << "New block pos: " << newBlockPos.x << ", " << newBlockPos.y << ", " << newBlockPos.z << std::endl;
			//std::cout << "==end==" << std::endl;

			glm::vec3 localIndex = GetBlockIndex(newBlockPos.x, newBlockPos.y, newBlockPos.z);

			int indexX = localIndex.x;
			int indexY = localIndex.y;
			int indexZ = localIndex.z;

			if (outOfBounds)
			{
				if (indexX < 0 && nebighours.left)
					nebighours.left->AddBlock(pos, id);
				else if (indexX >= CHUNK_SIZE && nebighours.right)
					nebighours.right->AddBlock(pos, id);
				else if (indexZ < 0 && nebighours.back)
					nebighours.back->AddBlock(pos, id);
				else if (indexZ >= CHUNK_SIZE && nebighours.front)
					nebighours.front->AddBlock(pos, id);
			}
			else
			{
				Block& newBlock = GetBlockActual(newBlockPos.x, newBlockPos.y, newBlockPos.z);
				newBlock.SetActive(true);
				newBlock.SetData(blockData.at(id));

				// Update the neighbours for AO
				if (indexX == 0 && nebighours.left)
					nebighours.left->dirty = true;
				if (indexX == CHUNK_SIZE - 1 && nebighours.right)
					nebighours.right->dirty = true;
				if (indexZ == 0 && nebighours.back)
					nebighours.back->dirty = true;
				if (indexZ == CHUNK_SIZE - 1 && nebighours.front)
					nebighours.front->dirty = true;

				dirty = true;
				success = true;
			}
		}

		return success;
	}

	/*
	* @brief remove a block from the chunk
	* @param pos: the position of the block
	* @return true if the block was removed, false otherwise
	*/
	bool Chunk::RemoveBlock(const glm::vec3& pos)
	{
		glm::vec3 newBlockPos = pos;

		if (!CheckBlockExists(pos.x, pos.y, pos.z, newBlockPos))
			return false;

		glm::vec3 localIndex = GetBlockIndex(newBlockPos.x, newBlockPos.y, newBlockPos.z);

		int indexX = localIndex.x;
		int indexY = localIndex.y;
		int indexZ = localIndex.z;

		Block& newBlock = GetBlockActual(newBlockPos.x, newBlockPos.y, newBlockPos.z);
		newBlock.SetActive(false);

		// Update the neighbours for AO
		if (indexX == 0 && nebighours.left)
			nebighours.left->dirty = true;
		if (indexX == CHUNK_SIZE - 1 && nebighours.right)
			nebighours.right->dirty = true;
		if (indexZ == 0 && nebighours.back)
			nebighours.back->dirty = true;
		if (indexZ == CHUNK_SIZE - 1 && nebighours.front)
			nebighours.front->dirty = true;

		dirty = true;
		return true;
	}

	/*
	* @brief remove multiple blocks from the chunk
	* @param positions: the positions of the blocks
	* @return true if the blocks were removed, false otherwise
	*/
	bool Chunk::RemoveBlocks(const std::vector<glm::vec3>& positions)
	{
		bool success = false;
		const std::unordered_map<int, BlockData>& blockData = ResourceHelper::GetBlockData();

		for (int i = 0; i < positions.size(); ++i)
		{
			const glm::vec3& pos = positions[i];

			bool outOfBounds = false;
			glm::vec3 newBlockPos = pos;

			if (!CheckBlockExists(pos.x, pos.y, pos.z, newBlockPos, outOfBounds))
				continue;

			glm::vec3 localIndex = GetBlockIndex(newBlockPos.x, newBlockPos.y, newBlockPos.z);

			int indexX = localIndex.x;
			int indexY = localIndex.y;
			int indexZ = localIndex.z;

			Block& newBlock = GetBlockActual(newBlockPos.x, newBlockPos.y, newBlockPos.z);
			newBlock.SetActive(false);

			// Update the neighbours for AO
			if (indexX == 0 && nebighours.left)
				nebighours.left->dirty = true;
			if (indexX == CHUNK_SIZE - 1 && nebighours.right)
				nebighours.right->dirty = true;
			if (indexZ == 0 && nebighours.back)
				nebighours.back->dirty = true;
			if (indexZ == CHUNK_SIZE - 1 && nebighours.front)
				nebighours.front->dirty = true;

			dirty = true;
			success = true;
		}
		return success;
	}

	/*
	* @brief debug the block
	* @param pos: the position of the block
	* @param id: the id of the block
	*/
	void Chunk::DebugBlock(const glm::vec3& pos, const unsigned int id)
	{		
		glm::vec3 blockPos;
		if (CheckBlockExists(pos.x, pos.y, pos.z, blockPos))
		{
			Block& block = GetBlockActual(blockPos.x, blockPos.y, blockPos.z);
		}
	}

	/*
	* @brief get the block at the specified coordinates
	* @param x: the x coordinate of the block
	* @param y: the y coordinate of the block
	* @param z: the z coordinate of the block
	* @return the block at the specified coordinates
	*/
	Block& Chunk::GetBlock(const int& x, const int& y, const int& z)
	{
		return (*this)(x, y, z);
	}
	const Block& Chunk::GetBlock(const int& x, const int& y, const int& z) const
	{
		return (*this)(x, y, z);
	}

	/*
	* @brief get the block at the specified world coordinates
	* @param x: the x coordinate in the world
	* @param y: the y coordinate in the world
	* @param z: the z coordinate in the world
	* @return the block at the specified world coordinates
	*/
	Block& Chunk::GetBlockActual(const float& x, const float& y, const float& z)
	{
		glm::vec3 localIndex = GetBlockIndex(x, y, z);

		int localX = localIndex.x;
		int localY = localIndex.y;
		int localZ = localIndex.z;

		if (localX < 0 || localX >= CHUNK_SIZE ||
			localY < 0 || localY >= CHUNK_HEIGHT ||
			localZ < 0 || localZ >= CHUNK_SIZE)
		{
			throw std::runtime_error("Block out of bounds");
		}

		return GetBlock(localX, localY, localZ);
	}
	const Block& Chunk::GetBlockActual(const float& x, const float& y, const float& z) const
	{
		glm::vec3 localIndex = GetBlockIndex(x, y, z);

		int localX = localIndex.x;
		int localY = localIndex.y;
		int localZ = localIndex.z;

		if (localX < 0 || localX >= CHUNK_SIZE ||
			localY < 0 || localY >= CHUNK_HEIGHT ||
			localZ < 0 || localZ >= CHUNK_SIZE)
		{
			throw std::runtime_error("Block out of bounds");
		}

		return GetBlock(localX, localY, localZ);
	}

	/*
	* @brief check if the block exists at the specified world coordinates
	* @param x: the x coordinate in the world
	* @param y: the y coordinate in the world
	* @param z: the z coordinate in the world
	* @param blockPos: the position of the block
	* @param outOfBounds: if the block is out of bounds
	* @return true if the block exists, false otherwise
	*/
	bool Chunk::CheckBlockExists(const float& x, const float& y, const float& z) const
	{
		glm::vec3 blockPos;
		bool outOfBounds = false;
		return CheckBlockExists(x, y, z, blockPos, outOfBounds);
	}
	bool Chunk::CheckBlockExists(const float& x, const float& y, const float& z, bool& outOfBounds) const
	{
		glm::vec3 blockPos;
		return CheckBlockExists(x, y, z, blockPos, outOfBounds);
		return false;
	}
	bool Chunk::CheckBlockExists(const float& x, const float& y, const float& z, glm::vec3& blockPos) const
	{
		bool outOfBounds = false;
		return CheckBlockExists(x, y, z, blockPos, outOfBounds);
	}
	bool Chunk::CheckBlockExists(const float& x, const float& y, const float& z, glm::vec3& blockPos, bool& outOfBounds) const
	{
		glm::vec3 localIndex = GetBlockIndex(x, y, z);
		outOfBounds = false;

		int indexX = localIndex.x;
		int indexY = localIndex.y;
		int indexZ = localIndex.z;

		float localX = (indexX + offset.x) * BLOCK_SIZE * 2;
		float localY = indexY * BLOCK_SIZE * 2;
		float localZ = (indexZ + offset.y) * BLOCK_SIZE * 2;

		blockPos = glm::vec3(localX, localY, localZ);

		if (indexX >= 0 && indexX < CHUNK_SIZE &&
			indexY >= 0 && indexY < CHUNK_HEIGHT &&
			indexZ >= 0 && indexZ < CHUNK_SIZE)
		{
			if (GetBlock(indexX, indexY, indexZ).IsActive())
				return true;
		}
		else
		{
			outOfBounds = true;
		}

		return false;
	}

	/*
	* @brief check if the block is in the chunk
	* @param x: the x coordinate of the block
	* @param y: the y coordinate of the block
	* @param z: the z coordinate of the block
	* @return true if the block is in the chunk, false otherwise
	*/
	bool Chunk::CheckBlockIsInChunk(const float& x, const float& y, const float& z) const
	{
		glm::vec3 localIndex = GetBlockIndex(x, y, z);

		int indexX = localIndex.x;
		int indexY = localIndex.y;
		int indexZ = localIndex.z;

		if (indexX >= 0 && indexX < CHUNK_SIZE &&
			indexY >= 0 && indexY < CHUNK_HEIGHT &&
			indexZ >= 0 && indexZ < CHUNK_SIZE)
		{
			return true;
		}

		return false;
	}

	/*
	* @brief check if the block exists by index
	* @param x: the x index of the block
	* @param y: the y index of the block
	* @param z: the z index of the block
	* @return true if the block exists, false otherwise
	*/
	bool Chunk::CheckBlockExistsByIndex(const float& x, const float& y, const float& z) const
	{
		if (x >= 0 && x < CHUNK_SIZE &&
			y >= 0 && y < CHUNK_HEIGHT &&
			z >= 0 && z < CHUNK_SIZE)
		{
			if (GetBlock(x, y, z).IsActive())
				return true;
		}
		return false;
	}

	/*
	* @brief initialize the terrain
	* @param offset: the offset of the terrain
	* @param noise: the noise map of the terrain
	*/
	void Chunk::InitTerrain(const glm::vec2& offset, const std::vector<float>& noiseMap, const std::vector<Chunk*>& nbs, bool is3DNoise)
	{
		if (noiseMap.size() != CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT)
			throw std::runtime_error("Noise map size does not match the chunk size");

		if(nbs.size() != 4)
			throw std::runtime_error("Neighbours size is not 4");

		// populate the neighbours
		nebighours.front = nbs[0];
		nebighours.back = nbs[1];
		nebighours.left = nbs[2];
		nebighours.right = nbs[3];

		this->offset = offset;

		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int z = 0; z < CHUNK_SIZE; ++z)
			{
				for (int y = 0; y < CHUNK_HEIGHT; ++y)
				{
					int index = x * CHUNK_SIZE * CHUNK_HEIGHT + z * CHUNK_HEIGHT + y;
					float noise = noiseMap[index];

					Block& block = GetBlock(x, y, z);

					// bedrock layer always active
					if (y < CHUNK_MIN_HEIGHT)
					{
						block.SetActive(true);
						InitBlocks(glm::vec3(x, y, z), block);
						continue;
					}

					if (is3DNoise)
					{
						if (noise > CHUNK_ACCEPTABLE_NOISE_3D)
						{
							block.SetActive(true);
							InitBlocks(glm::vec3(x, y, z), block);
						}
					}
					else
					{
						if (noise != 0)
						{
							block.SetActive(true);
							InitBlocks(glm::vec3(x, y, z), block);
						}
					}
				}
			}
		}
	}

	/*
	*  @brief initialize the environment objects like plants, trees, etc
	*/
	void Chunk::InitEnvironment()
	{
		const std::unordered_map<int, BlockData>& blockData = ResourceHelper::GetBlockData();

		// replace top dirt blocks with grass block
		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int z = 0; z < CHUNK_SIZE; ++z)
			{
				for (int y = CHUNK_SURFACE_HEIGHT; y < CHUNK_HEIGHT; ++y)
				{
					Block& block = GetBlock(x, y, z);
					if(block.GetData().id != 4)
						continue;

					// if it's dirt block and there is no block above it, replace it with grass block
					if (block.IsActive() && y+1 <= CHUNK_HEIGHT)
					{
						if(y + 1 == CHUNK_HEIGHT)
							block.SetData(blockData.at(5));
						else if (!GetBlock(x, y + 1, z).IsActive())
						{
							block.SetData(blockData.at(5));
						}
					}
				}
			}
		}

		// spawn trees
		SpawnTrees(8.5f);
		dirty = false;
	}

	/*
	* @brief create the mesh 
	* @brief using indices to reuse overlapping vertices
	* @brief culling faces that are not visible
	* @brief this way we can reduce the number of vertices
	* @param tex: the texture to use for the mesh
	*/
	void Chunk::CreateMesh(const Texture& tex)
	{
		if(mesh != nullptr)
			throw std::runtime_error("Mesh already created");

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		textures.push_back(tex);

		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				for (int z = 0; z < CHUNK_SIZE; ++z)
				{
					Block& block = GetBlock(x, y, z);

					if (block.IsActive())
					{
						FaceCulling faceCulling;

						if (block.GetData().shouldCullFace)
						{
							faceCulling = CullFaces(x, y, z);
						}

						float finalX = (x + offset.x) * BLOCK_SIZE * 2;
						float finalY = y * BLOCK_SIZE * 2;
						float finalZ = (z + offset.y) * BLOCK_SIZE * 2;
						CreateBlockData(finalX, finalY, finalZ, vertices, indices, faceCulling, block.GetData().textureIDs);
					}
				}
			}
		}

		mesh = new Mesh(vertices, indices, textures);
	}

	/*
	* @brief update the mesh
	*/
	void Chunk::UpdateMesh()
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				for (int z = 0; z < CHUNK_SIZE; ++z)
				{
					Block& block = GetBlock(x, y, z);

					if (block.IsActive())
					{
						FaceCulling faceCulling;

						if (block.GetData().shouldCullFace)
						{
							faceCulling = CullFaces(x, y, z);
						}

						float finalX = (x + offset.x) * BLOCK_SIZE * 2;
						float finalY = y * BLOCK_SIZE * 2;
						float finalZ = (z + offset.y) * BLOCK_SIZE * 2;
						CreateBlockData(finalX, finalY, finalZ, vertices, indices, faceCulling, block.GetData().textureIDs);
					}
				}
			}
		}

		mesh->Rebuild(vertices, indices);
	}

	/*
	* @brief get the offset of the chunk
	* @return the offset of the chunk
	*/
	const glm::vec2 Chunk::GetOffset() const
	{
		return offset;
	}

	/*
	* @brief get the texture index
	* @param pos: the position of the block
	* @param block: the block
	*/
	void Chunk::InitBlocks(const glm::vec3& pos, Block& block) const
	{
		const std::unordered_map<int, BlockData>& blockData = ResourceHelper::GetBlockData();

		// Init block data based on height
		if (pos.y <= CHUNK_MIN_HEIGHT)
		{
			block.SetData(blockData.at(0));
		}
		else if (pos.y <= 5.0f)
		{
			block.SetData(blockData.at(1));
		}
		else if (pos.y <= CHUNK_SURFACE_HEIGHT)
		{
			block.SetData(blockData.at(2));
		}
		else
		{
			block.SetData(blockData.at(4));
		}
	}

	/*
	* @brief create the block data
	* @param x: the x coordinate of the block
	* @param y: the y coordinate of the block
	* @param z: the z coordinate of the block
	* @param vertices: the vertices of the block
	* @param indices: the indices of the blockk
	* 
			p7 +--------------------+ p8
			   /|                   /|
		 ^    / |                  / |
	  y+ |   /  |                 /  |
			/   |                /   |
		p4 +--------------------+ p3 |   x+ ->
		   |    |               |    |
		   |    |               |    |
		   |    |               |    |
		   |    |               |    |    
		   | p6 +---------------|----+ p5
		   |   /                |   /
		   |  /                 |  /
		   | /                  | /
		   |/                   |/
		p1 +--------------------+ p2
		
				z+ |
				   v
	*/
	void Chunk::CreateBlockData(
		const float& x,
		const float& y,
		const float& z,
		std::vector<Vertex>& vertices,
		std::vector<unsigned int>& indices,
		const FaceCulling& faceCulling,
		std::vector<GLuint> texIndices)
	{
		// 8 vertices of a cube
		glm::vec3 p1 = glm::vec3(x - BLOCK_SIZE, y - BLOCK_SIZE, z + BLOCK_SIZE); // z+, bottom left
		glm::vec3 p2 = glm::vec3(x + BLOCK_SIZE, y - BLOCK_SIZE, z + BLOCK_SIZE); // z+, bottom right
		glm::vec3 p3 = glm::vec3(x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE); // z+, top right
		glm::vec3 p4 = glm::vec3(x - BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE); // z+, top left
		glm::vec3 p5 = glm::vec3(x + BLOCK_SIZE, y - BLOCK_SIZE, z - BLOCK_SIZE); // z-, bottom right
		glm::vec3 p6 = glm::vec3(x - BLOCK_SIZE, y - BLOCK_SIZE, z - BLOCK_SIZE); // z-, bottom left
		glm::vec3 p7 = glm::vec3(x - BLOCK_SIZE, y + BLOCK_SIZE, z - BLOCK_SIZE); // z-, top left
		glm::vec3 p8 = glm::vec3(x + BLOCK_SIZE, y + BLOCK_SIZE, z - BLOCK_SIZE); // z-, top right

		Vertex v;
		glm::vec3 pos = glm::vec3(x, y, z);
		glm::vec3 n = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec2 uv = glm::vec2(0.0f, 0.0f);
		float offset = BLOCK_SIZE * 2.0f;

		glm::vec4 occlusion = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// =====  Front face =====
		if (!faceCulling.front)
		{
			n = glm::vec3(0.0f, 0.0f, 1.0f);
			occlusion = glm::vec4
			(
				GetOcclusionValue(pos, -offset, 0, offset) * GetOcclusionValue(pos, 0, -offset, offset) * GetOcclusionValue(pos, -offset, -offset, offset),
				GetOcclusionValue(pos, offset, 0, offset) * GetOcclusionValue(pos, 0, -offset, offset) * GetOcclusionValue(pos, offset, -offset, offset),
				GetOcclusionValue(pos, offset, 0, offset) * GetOcclusionValue(pos, 0, offset, offset) * GetOcclusionValue(pos, offset, offset, offset),
				GetOcclusionValue(pos, -offset, 0, offset) * GetOcclusionValue(pos, 0, offset, offset) * GetOcclusionValue(pos, -offset, offset, offset)
			);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p1, n, uv, occlusion[0], texIndices[0] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p2, n, uv, occlusion[1], texIndices[0] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p3, n, uv, occlusion[2], texIndices[0] };
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p4, n, uv, occlusion[3], texIndices[0] };
			vertices.push_back(v);


			// create indices for the face (2 triangles), front: p1, p2, p3, p1, p3, p4
			if (occlusion[0] + occlusion[2] > occlusion[1] + occlusion[3])
			{
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
			}
			else // if the diagnal AO is greater than other diagnal, flip the quad, front: p4, p1, p2, p4, p2, p3
			{
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
			}
		}

		// =====  Back face =====
		if (!faceCulling.back)
		{
			n = glm::vec3(0.0f, 0.0f, -1.0f);
			occlusion = glm::vec4
			(
				GetOcclusionValue(pos, offset, 0, -offset) * GetOcclusionValue(pos, 0, -offset, -offset) * GetOcclusionValue(pos, offset, -offset, -offset),
				GetOcclusionValue(pos, -offset, 0, -offset) * GetOcclusionValue(pos, 0, -offset, -offset) * GetOcclusionValue(pos, -offset, -offset, -offset),
				GetOcclusionValue(pos, -offset, 0, -offset) * GetOcclusionValue(pos, 0, offset, -offset) * GetOcclusionValue(pos, -offset, offset, -offset),
				GetOcclusionValue(pos, offset, 0, -offset) * GetOcclusionValue(pos, 0, offset, -offset) * GetOcclusionValue(pos, offset, offset, -offset)
			);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p5, n, uv, occlusion[0], texIndices[1] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p6, n, uv, occlusion[1], texIndices[1] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p7, n, uv, occlusion[2], texIndices[1]};
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p8, n, uv, occlusion[3], texIndices[1]};
			vertices.push_back(v);

			// back: p5, p6, p7, p5, p7, p8
			if (occlusion[0] + occlusion[2] > occlusion[1] + occlusion[3])
			{
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
			}
			else // if the diagnal AO is greater than other diagnal, flip the quad, back: p8, p5, p6, p8, p6, p7
			{
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
			}
		}

		// ===== Right face ===== 
		if (!faceCulling.right)
		{
			n = glm::vec3(1.0f, 0.0f, 0.0f);
			occlusion = glm::vec4
			(
				GetOcclusionValue(pos, offset, 0, offset) * GetOcclusionValue(pos, offset, -offset, 0) * GetOcclusionValue(pos, offset, -offset, offset),
				GetOcclusionValue(pos, offset, 0, -offset) * GetOcclusionValue(pos, offset, -offset, 0) * GetOcclusionValue(pos, offset, -offset, -offset),
				GetOcclusionValue(pos, offset, 0, -offset) * GetOcclusionValue(pos, offset, offset, 0) * GetOcclusionValue(pos, offset, offset, -offset),
				GetOcclusionValue(pos, offset, 0, offset) * GetOcclusionValue(pos, offset, offset, 0) * GetOcclusionValue(pos, offset, offset, offset)
			);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p2, n, uv, occlusion[0], texIndices[2] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p5, n, uv, occlusion[1], texIndices[2] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p8, n, uv, occlusion[2], texIndices[2]};
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p3, n, uv, occlusion[3], texIndices[2]};
			vertices.push_back(v);

			// right: p2, p5, p8, p2, p8, p3
			if (occlusion[0] + occlusion[2] > occlusion[1] + occlusion[3])
			{
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
			}
			else // if the diagnal AO is greater than other diagnal, flip the quad, right: p3, p2, p5, p3, p5, p8
			{
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
			}

		}

		// =====  Left face =====
		if (!faceCulling.left)
		{
			n = glm::vec3(-1.0f, 0.0f, 0.0f);
			occlusion = glm::vec4
			(
				GetOcclusionValue(pos, -offset, 0, -offset) * GetOcclusionValue(pos, -offset, -offset, 0) * GetOcclusionValue(pos, -offset, -offset, -offset),
				GetOcclusionValue(pos, -offset, 0, offset) * GetOcclusionValue(pos, -offset, -offset, 0) * GetOcclusionValue(pos, -offset, -offset, offset),
				GetOcclusionValue(pos, -offset, 0, offset) * GetOcclusionValue(pos, -offset, offset, 0) * GetOcclusionValue(pos, -offset, offset, offset),
				GetOcclusionValue(pos, -offset, 0, -offset) * GetOcclusionValue(pos, -offset, offset, 0) * GetOcclusionValue(pos, -offset, offset, -offset)
			);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p6, n, uv, occlusion[0], texIndices[3] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p1, n, uv, occlusion[1], texIndices[3] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p4, n, uv, occlusion[2], texIndices[3]};
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p7, n, uv, occlusion[3], texIndices[3]};
			vertices.push_back(v);

			// left: p6, p1, p4, p6, p4, p7
			if (occlusion[0] + occlusion[2] > occlusion[1] + occlusion[3])
			{
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
			}
			else // if the diagnal AO is greater than other diagnal, flip the quad, left: p7, p4, p1, p7, p1, p6
			{
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
			}
		}

		//=====  Top face =====
		if (!faceCulling.top)
		{
			n = glm::vec3(0.0f, 1.0f, 0.0f);
			occlusion = glm::vec4
			(
				GetOcclusionValue(pos, 0, offset, offset) * GetOcclusionValue(pos, -offset, offset, 0) * GetOcclusionValue(pos, -offset, offset, offset),
				GetOcclusionValue(pos, 0, offset, offset) * GetOcclusionValue(pos, offset, offset, 0) * GetOcclusionValue(pos, offset, offset, offset),
				GetOcclusionValue(pos, 0, offset, -offset) * GetOcclusionValue(pos, offset, offset, 0) * GetOcclusionValue(pos, offset, offset, -offset),
				GetOcclusionValue(pos, 0, offset, -offset) * GetOcclusionValue(pos, -offset, offset, 0) * GetOcclusionValue(pos, -offset, offset, -offset)
			);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p4, n, uv, occlusion[0], texIndices[4] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p3, n, uv, occlusion[1], texIndices[4] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p8, n, uv, occlusion[2], texIndices[4]};
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p7, n, uv, occlusion[3], texIndices[4]};
			vertices.push_back(v);

			// top: p4, p3, p8, p4, p8, p7
			if (occlusion[0] + occlusion[2] > occlusion[1] + occlusion[3])
			{
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
			}
			else // if the diagnal AO is greater than other diagnal, flip the quad, top: p7, p8, p3, p7, p3, p4
			{
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
			}
		}

		// =====  Bottom face ===== 
		if (!faceCulling.bottom)
		{
			n = glm::vec3(0.0f, -1.0f, 0.0f);
			occlusion = glm::vec4
			(
				GetOcclusionValue(pos, 0, -offset, -offset) * GetOcclusionValue(pos, -offset, -offset, 0) * GetOcclusionValue(pos, -offset, -offset, -offset),
				GetOcclusionValue(pos, 0, -offset, -offset) * GetOcclusionValue(pos, offset, -offset, 0) * GetOcclusionValue(pos, offset, -offset, -offset),
				GetOcclusionValue(pos, 0, -offset, offset) * GetOcclusionValue(pos, offset, -offset, 0) * GetOcclusionValue(pos, offset, -offset, offset),
				GetOcclusionValue(pos, 0, -offset, offset) * GetOcclusionValue(pos, -offset, -offset, 0) * GetOcclusionValue(pos, -offset, -offset, offset)
			);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p6, n, uv, occlusion[0], texIndices[5] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p5, n, uv, occlusion[1], texIndices[5] };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p2, n, uv, occlusion[2], texIndices[5]};
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p1, n, uv, occlusion[3], texIndices[5]};
			vertices.push_back(v);

			// bottom: p6, p5, p2, p6, p2, p1
			if (occlusion[0] + occlusion[2] > occlusion[1] + occlusion[3])
			{
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
			}
			else // if the diagnal AO is greater than other diagnal, flip the quad, bottom: p1, p2, p5, p1, p5, p6
			{
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
			}
		}
	}

	/*
	* @brief get the block index in this chunk at the specified world coordinates
	* @param x: the x coordinate in the world
	* @param y: the y coordinate in the world
	* @param z: the z coordinate in the world
	* @return the block index at the specified world coordinates
	*/
	glm::vec3 Chunk::GetBlockIndex(const float& x, const float& y, const float& z) const
	{
		int localX = round(x - (offset.x * BLOCK_SIZE * 2));
		int localY = round(y);
		int localZ = round(z - (offset.y * BLOCK_SIZE * 2));

		return glm::vec3(localX, localY, localZ);
	}

	/*
	* @brief get the face culling of the block
	* @param x: the x coordinate of the block
	* @param y: the y coordinate of the block
	* @param z: the z coordinate of the block
	* @return the face culling of the block
	*/
	Chunk::FaceCulling Chunk::CullFaces(const float& x, const float& y, const float& z)
	{
		FaceCulling faceCulling;

		// check if neighbours need to be culled
		if (x > 0) faceCulling.left = GetBlock(x - 1, y, z).IsActive() && GetBlock(x - 1, y, z).GetData().shouldCullFace;
		if (x < CHUNK_SIZE - 1) faceCulling.right = GetBlock(x + 1, y, z).IsActive() && GetBlock(x + 1, y, z).GetData().shouldCullFace;

		if (y > 0) faceCulling.bottom = GetBlock(x, y - 1, z).IsActive() && GetBlock(x, y - 1, z).GetData().shouldCullFace;
		if (y < CHUNK_HEIGHT - 1) faceCulling.top = GetBlock(x, y + 1, z).IsActive() && GetBlock(x, y + 1, z).GetData().shouldCullFace;

		if (z > 0) faceCulling.back = GetBlock(x, y, z - 1).IsActive() && GetBlock(x, y, z - 1).GetData().shouldCullFace;
		if (z < CHUNK_SIZE - 1) faceCulling.front = GetBlock(x, y, z + 1).IsActive() && GetBlock(x, y, z + 1).GetData().shouldCullFace;

		// check if border blocks need to be culled (only x and z)
		if (x == 0 && nebighours.left != nullptr) faceCulling.left = nebighours.left->GetBlock(CHUNK_SIZE - 1, y, z).IsActive();
		if (x == CHUNK_SIZE - 1 && nebighours.right != nullptr) faceCulling.right = nebighours.right->GetBlock(0, y, z).IsActive();

		if (z == 0 && nebighours.back != nullptr) faceCulling.back = nebighours.back->GetBlock(x, y, CHUNK_SIZE - 1).IsActive();
		if (z == CHUNK_SIZE - 1 && nebighours.front != nullptr) faceCulling.front = nebighours.front->GetBlock(x, y, 0).IsActive();

		return faceCulling;
	}

	/*
	* @brief get the occlusion data of the block
	* @param indices: the indices of the block
	* @param xOffset: the x offset
	* @param yOffset: the y offset
	* @param zOffset: the z offset
	* @return the AO value 
	*/
	float Chunk::GetOcclusionValue(const glm::vec3& pos, const float& xOffset, const float& yOffset, const float& zOffset)
	{
		bool outOfBounds = false;
		glm::vec3 posToCheck = glm::vec3(pos.x + xOffset, pos.y + yOffset, pos.z + zOffset);

		if (CheckBlockExists(posToCheck.x, posToCheck.y, posToCheck.z, outOfBounds))
		{
			return 0.75f;
		}
		else if (outOfBounds)
		{
			glm::vec3 localIndex = GetBlockIndex(posToCheck.x, posToCheck.y, posToCheck.z);

			int indexX = localIndex.x;
			int indexY = localIndex.y;
			int indexZ = localIndex.z;

			if (indexX < 0 && nebighours.left)
				return nebighours.left->GetOcclusionValue(pos, xOffset, yOffset, zOffset);
			else if (indexX >= CHUNK_SIZE && nebighours.right)
				return nebighours.right->GetOcclusionValue(pos, xOffset, yOffset, zOffset);
			else if (indexZ < 0 && nebighours.back)
				return nebighours.back->GetOcclusionValue(pos, xOffset, yOffset, zOffset);
			else if (indexZ >= CHUNK_SIZE && nebighours.front)
				return nebighours.front->GetOcclusionValue(pos, xOffset, yOffset, zOffset);
		}
		return 1.0f;
	}

	/*
	* @brief create a tree at the specified position
	* @param pos: the position of the tree
	* @param minHeight: the minimum height of the tree
	* @param maxHeight: the maximum height of the tree
	* @return true if the tree was created, false otherwise
	*/
	bool Chunk::CreateTree(const glm::vec3& pos, const int& minHeight, const int& maxHeight)
	{
		// Randomized tree height
		int trunkHeight = Core::CommonUtils::RandomRange(minHeight, maxHeight);
		std::vector<glm::vec3> positions;
		std::vector<unsigned int> ids;

		// Place the trunk blocks (vertical column)
		for (int i = 0; i < trunkHeight; ++i) 
		{
			if (pos.y + i > CHUNK_HEIGHT)
				continue;

			positions.push_back(glm::vec3(pos.x, pos.y + i, pos.z));
			ids.push_back(6);
		}

		// Generate the leaf canopy (a cube of leaf blocks)
		int canopySize = 3;  // Canopy spread  
		int leafStart = trunkHeight - 1;
		int leafStop = trunkHeight + 2;
		int trim = canopySize;
		int trimStart = trunkHeight;

		for (int y = leafStart; y <= leafStop; ++y)
		{
			for (int x = -canopySize; x <= canopySize; ++x)
			{
				for (int z = -canopySize; z <= canopySize; ++z)
				{
					int xAbs = std::abs(x);
					int zAbs = std::abs(z);

					if (y >= trimStart)
					{
						// trim leaf blocks the higher they are
						if (xAbs > trim || zAbs > trim)
							continue;
					}

					// trim corner leaf blocks
					if (x != 0 && z != 0 && (xAbs == trim || zAbs == trim))
					{
						if (x == z)
							continue;

						if (std::abs(xAbs - zAbs) == 0)
							continue;
					}

					positions.push_back(glm::vec3(pos.x + x, pos.y + y, pos.z + z));
					ids.push_back(7);
				}
			}	
			
			trim = y >= trimStart ? trim - 1 : trim;
		}

		return AddBlocks(positions, ids);
	}

	/*
	* @brief spawn trees in the chunk
	* @param distBetweenTrees: the distance between trees
	*/
	void Chunk::SpawnTrees(const float distBetweenTrees)
	{
		glm::vec2 sampleRegionSize = glm::vec2(CHUNK_SIZE * BLOCK_SIZE * 2 - 1, CHUNK_SIZE * BLOCK_SIZE * 2 - 1);
		std::vector<glm::vec2> treePositions = Core::CommonUtils::PoissonDiscSampling(distBetweenTrees, sampleRegionSize, 50);

		for (auto& pos : treePositions)
		{
			for (int y = 10; y < CHUNK_MAX_HEIGHT; ++y)
			{
				if (y + 5 <= CHUNK_HEIGHT 
					&& !GetBlock(pos.x, y + 7, pos.y).IsActive()
					&& !GetBlock(pos.x, y + 5, pos.y).IsActive()
					&& !GetBlock(pos.x, y + 1, pos.y).IsActive())
				{
					float worldX = (pos.x + offset.x) * BLOCK_SIZE * 2;
					float worldZ = (pos.y + offset.y) * BLOCK_SIZE * 2;
					
					glm::vec3 blockPos = glm::vec3(0);
					if (!CheckBlockExists(worldX, y, worldZ, blockPos))
						continue;

					CreateTree(blockPos, 5, 7);
					break;
				}
			}
		}

	}
}
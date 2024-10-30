#pragma once
#include "core/world/Block.h"
#include "core/render/Mesh.h"

namespace VoxelWasteland::Core
{
	class CustomShader;

	class Chunk
	{
	public:
		Chunk();
		~Chunk();

		void Update();
		void Draw(CustomShader* shader);

		Block& operator()(const int& x, const int& y, const int& z);
		const Block& operator()(const int& x, const int& y, const int& z) const;

		bool TryAddBlock(const glm::vec3& pos, const unsigned int id);
		bool TryAddBlocks(const glm::vec3& hit, const std::vector<glm::vec3>& pos, const std::vector<unsigned int>& ids);
		bool AddBlock(const glm::vec3& pos, const unsigned int id);
		bool AddBlocks(const std::vector<glm::vec3>& pos, const const std::vector<unsigned int> ids);

		bool RemoveBlock(const glm::vec3& pos);
		bool RemoveBlocks(const std::vector<glm::vec3>& pos);

		void DebugBlock(const glm::vec3& pos, const unsigned int id);

		Block& GetBlock(const int& x, const int& y, const int& z);
		const Block& GetBlock(const int& x, const int& y, const int& z) const;

		Block& GetBlockActual(const float& x, const float& y, const float& z);
		const Block& GetBlockActual(const float& x, const float& y, const float& z) const;

		bool CheckBlockExists(const float& x, const float& y, const float& z) const;
		bool CheckBlockExists(const float& x, const float& y, const float& z, bool& outOfBounds) const;
		bool CheckBlockExists(const float& x, const float& y, const float& z, glm::vec3& blockPos) const;
		bool CheckBlockExists(const float& x, const float& y, const float& z, glm::vec3& blockPos, bool& outOfBounds) const;

		bool CheckBlockIsInChunk(const float& x, const float& y, const float& z) const;
		bool CheckBlockExistsByIndex(const float& x, const float& y, const float& z) const;

		void InitTerrain(const glm::vec2& offset, const std::vector<float>& noise, const std::vector<Chunk*>& nbs, bool is3DNoise);
		void InitEnvironment();

		void CreateMesh(const Texture& tex);
		void UpdateMesh();

		const glm::vec2 GetOffset() const;

	private:
		struct FaceCulling
		{
			bool front = false;
			bool back = false;
			bool left = false;
			bool right = false;
			bool top = false;
			bool bottom = false;
		};

		struct Nebighours
		{
			Chunk* front = nullptr;
			Chunk* back = nullptr;
			Chunk* left = nullptr;
			Chunk* right = nullptr;
		};

		void CreateBlockData(
			const float& x,
			const float& y,
			const float& z,
			std::vector<Vertex>& vertices, 
			std::vector<unsigned int>& indices,
			const FaceCulling& faceCulling,
			std::vector<GLuint> texIndices
		);

		void InitBlocks(const glm::vec3& pos, Block& block) const;
		glm::vec3 GetBlockIndex(const float& x, const float& y, const float& z) const;

		FaceCulling CullFaces(const float& x, const float& y, const float& z);
		float GetOcclusionValue(const glm::vec3& pos, const float& xOffset, const float& yOffset, const float& zOffset);

		bool CreateTree(const glm::vec3& pos, const int& minHeight = 4, const int& maxHeight = 6);
		void SpawnTrees(const float distBetweenTrees);

		// ====== properties ======

		std::vector<Block> blocks;
		Mesh* mesh;

		// terrain generation
		glm::vec2 offset;
		Nebighours nebighours;

		bool dirty = false;
	};
}
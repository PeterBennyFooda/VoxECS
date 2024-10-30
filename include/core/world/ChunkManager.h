#pragma once
#include "core/world/Chunk.h"

namespace VoxelWasteland::Core
{
	class ChunkManager
	{
	public:
		ChunkManager();
		ChunkManager(bool async, bool noise3D);
		~ChunkManager();

		void GenerateChunks(std::vector<Chunk*>& chunks, const glm::vec3& playerPos);

	private:
		std::vector<float> GetNoiseMap(int seed);
		std::vector<float> GetNoiseMap3D(int seed);

		void ProcessNoiseMap(std::vector<float>& noiseMap, std::vector<Chunk*>& chunks, const glm::vec3& playerPos);
		void SmoothNoiseMap(std::vector<float>& noiseMap1, std::vector<float>& noiseMap2);
		void HeightMapBlend(std::vector<float>& noiseMap1, std::vector<float>& heightMap);

		std::vector<Chunk*> GetChunks(const int& size);
		const Texture GetChunkTexture() const;

		bool isAsync;
		bool is3DNoise;

		int currentSeed1 = 1337;
		int currentSeed2 = 9658;
	};
}
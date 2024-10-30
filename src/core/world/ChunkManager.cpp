#include "core/world/ChunkManager.h"
#include "core/ResourceHelper.h"

#include "FastNoiseLite.h"

namespace VoxelWasteland::Core
{
	ChunkManager::ChunkManager() : isAsync(true), is3DNoise(false)
	{
	}

	ChunkManager::ChunkManager(bool async, bool noise3D) : isAsync(async), is3DNoise(noise3D)
    {
    }

	ChunkManager::~ChunkManager()
	{
	}

	void ChunkManager::GenerateChunks(std::vector<Chunk*>& chunks, const glm::vec3& playerPos)
	{
		// get noise map
		std::vector<float> heightMap = GetNoiseMap(currentSeed1);
		std::vector<float> noiseMap1 = is3DNoise ? GetNoiseMap3D(currentSeed1) : GetNoiseMap(currentSeed1);
        std::vector<float> noiseMap2 = is3DNoise ? GetNoiseMap3D(currentSeed2) : GetNoiseMap(currentSeed2);

        // Divide work across threads
        chunks.reserve(MAP_SIZE * MAP_SIZE); // Reserve memory to avoid reallocations

        if (isAsync)
        {
            // produce chunks in parallel
            std::vector<std::future<std::vector<Chunk*>>> futures;

            int threadCount = 8;
            int objectCount = MAP_SIZE * MAP_SIZE;
            int threadSize = objectCount / threadCount;

            for (int i = 0; i < threadCount; ++i)
            {
				futures.push_back(std::async(std::launch::async, &ChunkManager::GetChunks, this, threadSize));
            }

            for (auto& future : futures)
            {
				std::vector<Chunk*> threadChunks = future.get();
				chunks.insert(chunks.end(), threadChunks.begin(), threadChunks.end());
            }
        }
        else
        {
			chunks = GetChunks(MAP_SIZE * MAP_SIZE);
        }

        // noise map
        SmoothNoiseMap(noiseMap1, noiseMap2);

        if(is3DNoise)
		    HeightMapBlend(noiseMap1, heightMap);

		ProcessNoiseMap(noiseMap1, chunks, playerPos);

        // get textures for meshes
		const Texture tex = GetChunkTexture();

		// initialize the environment like trees, grass, etc.
        for (auto& chunk : chunks)
		{
			chunk->InitEnvironment();
        }

		// finally, create the mesh
        for (auto& chunk : chunks)
        {
			chunk->CreateMesh(tex);
        }
	}

    /*
	* @brief Generate a noise map for the chunks
	* @return std::vector<float> noiseMap
    */
    std::vector<float> ChunkManager::GetNoiseMap(int seed)
    {
        FastNoiseLite noise(seed);
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

        int blockCount = CHUNK_SIZE * CHUNK_SIZE * MAP_SIZE * MAP_SIZE;
        std::vector<float> noiseMap = std::vector<float>();
        noiseMap = std::vector<float>(blockCount);

        for (int x = 0; x < CHUNK_SIZE * MAP_SIZE; ++x)
        {
            for (int z = 0; z < CHUNK_SIZE * MAP_SIZE; ++z)
            {
                noiseMap[x * CHUNK_SIZE * MAP_SIZE + z] = noise.GetNoise((float)x, (float)z) * CHUNK_NOISE_SCALE;
            }
        }

        // Map to 3d array
		std::vector<float> resultMap = std::vector<float>(CHUNK_SIZE * CHUNK_SIZE * MAP_SIZE * MAP_SIZE * CHUNK_HEIGHT);
        for (int x = 0; x < CHUNK_SIZE * MAP_SIZE; ++x)
        {
            for (int z = 0; z < CHUNK_SIZE * MAP_SIZE; ++z)
            {
                int index = x * CHUNK_SIZE * MAP_SIZE + z;
                int heightDifference = CHUNK_HEIGHT - CHUNK_MIN_HEIGHT;
                int height = CHUNK_MIN_HEIGHT + (int)(noiseMap[index] * heightDifference);

                if(height < 0)
					height = 0;

				if(height > CHUNK_HEIGHT)
					height = CHUNK_HEIGHT;

                for (int y = 0; y < height; ++y)
                {
					int newIndex = x * CHUNK_SIZE * MAP_SIZE * CHUNK_HEIGHT + z * CHUNK_HEIGHT + y;
					resultMap[newIndex] = noiseMap[index];
                }
            }
        }

        return resultMap;
    }

    /*
	* @brief Generate a 3D noise map for the chunks
	* @return std::vector<float> noiseMap
    */
    std::vector<float> ChunkManager::GetNoiseMap3D(int seed)
    {
        FastNoiseLite noise(seed);
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

		int blockCount = CHUNK_SIZE * CHUNK_SIZE * MAP_SIZE * MAP_SIZE * CHUNK_HEIGHT;
        std::vector<float> noiseMap = std::vector<float>();
        noiseMap = std::vector<float>(blockCount);

		for (int x = 0; x < CHUNK_SIZE * MAP_SIZE; ++x)
		{
			for (int z = 0; z < CHUNK_SIZE * MAP_SIZE; ++z)
			{
				for (int y = 0; y < CHUNK_MAX_HEIGHT; ++y)
				{
					noiseMap[x * CHUNK_SIZE * MAP_SIZE * CHUNK_HEIGHT + z * CHUNK_HEIGHT + y] = noise.GetNoise((float)x, (float)y, (float)z) * CHUNK_NOISE_SCALE_3D;
				}
			}
		}

        return noiseMap;
    }

    /*
	* @brief Process the noise map
    */
    void ChunkManager::ProcessNoiseMap(std::vector<float>& noiseMap, std::vector<Chunk*>& chunks, const glm::vec3& playerPos)
    {
        for (int index = 0; index < MAP_SIZE * MAP_SIZE; ++index)
        {
            int x = index / MAP_SIZE;  // Extract x from the combined index
            int z = index % MAP_SIZE;  // Extract z from the combined index

			// Calculate the offset of the chunk in world space
			// Based on the player's position and block size
			int xOffset = x * CHUNK_SIZE - MAP_SIZE * CHUNK_SIZE / 2;
            int zOffset = z * CHUNK_SIZE - MAP_SIZE * CHUNK_SIZE / 2;


            glm::vec2 offset = glm::vec2(xOffset, zOffset);
            std::vector<float> subNoiseMap;

            subNoiseMap.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT);  // Reserve memory to avoid reallocations

            // Flattened inner loop
            for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT; ++i)
            {
                int localX = i / (CHUNK_SIZE * CHUNK_HEIGHT);  // i divided by CHUNK_SIZE gives x-axis index within the chunk
                int localZ = (i / CHUNK_HEIGHT) % CHUNK_SIZE;  // i mod CHUNK_SIZE gives z-axis index within the chunk
                int localY = i % CHUNK_HEIGHT;  // i mod CHUNK_HEIGHT gives y-axis index within the chunk

                int noiseIndex = (x * CHUNK_SIZE + localX) * CHUNK_SIZE * MAP_SIZE * CHUNK_HEIGHT + (z * CHUNK_SIZE + localZ) * CHUNK_HEIGHT + localY;
                subNoiseMap.push_back(noiseMap[noiseIndex]);
            }

            // Get nb chunks in order: front, back, left, right
            std::vector<Chunk*> nbs(4, nullptr);
            if (z < MAP_SIZE - 1) nbs[0] = chunks[index + 1];  // Front
            if (z > 0) nbs[1] = chunks[index - 1];  // Back
            if (x > 0) nbs[2] = chunks[index - MAP_SIZE];  // Left
            if (x < MAP_SIZE - 1) nbs[3] = chunks[index + MAP_SIZE]; // Right

            chunks[index]->InitTerrain(offset, subNoiseMap, nbs, is3DNoise);  // Use the 1D index for the chunk array
        }
    }

    /*
	* @brief Smooth the noise map with linear interpolation
	* @param std::vector<float>& noiseMap1: first noise map
	* @param std::vector<float>& noiseMap2: second noise map
    */
    void ChunkManager::SmoothNoiseMap(std::vector<float>& noiseMap1, std::vector<float>& noiseMap2)
    {
		int size = std::min(noiseMap1.size(), noiseMap2.size());
     
        float time = 0.0f;
        for (int i = 0; i < size; ++i)
        {      
			time = i / (float)size;
            time = glm::smoothstep(0.0f, 1.0f, time);
            noiseMap1[i] = noiseMap1[i] * (1.0 - time) + (noiseMap2[i] * time);
        }
    }

    /*
	* @brief Blend the height map with the noise map
	* @param std::vector<float>& noiseMap1: first noise map
	* @param std::vector<float>& heightMap: height map
    */
    void ChunkManager::HeightMapBlend(std::vector<float>& noiseMap1, std::vector<float>& heightMap)
    {
        if (noiseMap1.size() != heightMap.size())
            return;

        float time = 0.0f;
		for (int i = 0; i < heightMap.size(); ++i)
		{         
            if (heightMap[i] != 0)
            {
                time = i / (float)heightMap.size();
                time = glm::smoothstep(0.0f, 1.0f, time);
                noiseMap1[i] = noiseMap1[i] * (1.0 - time) + (heightMap[i] * time);
            }       
		}
    }

    /*
	* @brief Generate chunks
	* @param const int& size: size of the chunks
	* @return std::vector<Chunk*>& chunks
    */
    std::vector<Chunk*> ChunkManager::GetChunks(const int& size)
    {
        std::vector<Chunk*> chunks;
		chunks.reserve(size);

        for (int j = 0; j < size; ++j)
        {
            //sf::Clock clock;
            chunks.emplace_back(new Chunk());
			//std::cout << "Time to create chunk: " << clock.getElapsedTime().asSeconds() << "s" << std::endl;
        }

		return chunks;
    }

    /*
	* @brief Get the chunk texture
	* @return const Texture tex
    */
    const Texture ChunkManager::GetChunkTexture() const
    {
        std::unordered_map<int, BlockData> blockData = ResourceHelper::GetBlockData();
        std::vector<std::string> paths;

        for (auto& data : blockData)
        {
            int id = data.first;
            std::vector<std::string> subPaths = ResourceHelper::GetDefaultTextures(id);
            paths.insert(paths.end(), subPaths.begin(), subPaths.end());
        }

        unsigned int textureArrID = ResourceHelper::LoadTextureArray(paths);
        Texture tex = { textureArrID, DEFAUL_TEXTURE_TYPE };

        return tex;
    }
}
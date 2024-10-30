#pragma once

namespace VoxelWasteland::Core
{
	class CommonUtils
	{
	public:
		
		/*
		* @brief Generates a random number between min and max
		* @param min The minimum value
		* @param max The maximum value
		* @return A random integer number between min and max (inclusive)
		*/
		inline static int RandomRange(const int min, const int max)
		{
			std::random_device rd;   
			std::mt19937 gen(rd());  
			std::uniform_int_distribution<> dist(min, max);

			return dist(gen);
		}

		/*
		* @brief Generates a random number between min and max
		* @param min The minimum value
		* @param max The maximum value
		* @return A random float number between min and max (min inclusive, max exclusive)
		*/
		inline static float RandomRange(const float min, const float max)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dist(min, max);

			return dist(gen);
		}

		/*
		* @brief Get a list of points that are generated using Poisson Disc Sampling
		* @param radius the diagonal of the square cell
		* @param sampleRegionSize the size of the region to sample
		* @param numSamplesBeforeRejection the number of samples to try before rejecting a point
		* @return a list of points that are generated using Poisson Disc Sampling
		*/
		inline static std::vector<glm::vec2> PoissonDiscSampling(const float radius, const glm::vec2& sampleRegionSize, const int numSamplesBeforeRejection = 30)
		{
			// radius is the diagonal of the square cell, to get the side length we need to divide it by sqrt(2)
			float cellSize = radius / sqrt(2);

			int gridSizeX = ceil(sampleRegionSize.x / cellSize);
			int gridSizeY = ceil(sampleRegionSize.y / cellSize);
			std::vector<int> grid(gridSizeX * gridSizeY, -1);
			std::vector<glm::vec2> points;
			std::vector<glm::vec2> spawnPoints;

			// start from the center of the region
			spawnPoints.push_back(glm::vec2(sampleRegionSize.x / 2, sampleRegionSize.y / 2));
			while (spawnPoints.size() > 0)
			{
				int spawnIndex = RandomRange(0, spawnPoints.size()-1);
				glm::vec2 spawnCenter = spawnPoints[spawnIndex];
				bool accepted = false;

				for (int i = 0; i < numSamplesBeforeRejection; ++i)
				{
					float angle = RandomRange(0.0f, 2.0f * M_PI);
					glm::vec2 direction = glm::vec2(sin(angle), cos(angle));
					glm::vec2 candidate = spawnCenter + direction * RandomRange(radius, 2 * radius); // candidate is a point within the annulus

					if (IsValid(candidate, sampleRegionSize, cellSize, radius, points, grid))
					{
						int girdIndex = gridSizeX * (int)(candidate.y / cellSize) + (int)(candidate.x / cellSize);
						if (grid[girdIndex] >= 0)
							continue;

						points.push_back(candidate);
						spawnPoints.push_back(candidate);

						grid[girdIndex] = points.size() - 1; // store the index of the point in the grid
						accepted = true;
						break;
					}
				}

				if (!accepted)
				{
					spawnPoints.erase(spawnPoints.begin() + spawnIndex);
				}
			}
			return points;
		}

	private:

		inline static bool IsValid(glm::vec2 candidate, glm::vec2 sampleRegionSize, float cellSize, float radius, std::vector<glm::vec2> points, std::vector<int> grid)
		{
			if (candidate.x >= 0 && candidate.x < sampleRegionSize.x && candidate.y >= 0 && candidate.y < sampleRegionSize.y)
			{
				int cellX = (int)(candidate.x / cellSize);
				int cellY = (int)(candidate.y / cellSize);
				int searchStartX = std::max(0, cellX - 2);
				int searchEndX = std::min(cellX + 2, (int)(sampleRegionSize.x / cellSize - 1));
				int searchStartY = std::max(0, cellY - 2);
				int searchEndY = std::min(cellY + 2, (int)(sampleRegionSize.y / cellSize - 1));

				for (int x = searchStartX; x <= searchEndX; x++) 
				{
					for (int y = searchStartY; y <= searchEndY; y++) 
					{
						int pointIndex = grid[x, y] - 1;
						if (pointIndex >= 0) 
						{
							// squared length of the vector
							float sqrDst = glm::length2(candidate - points[pointIndex]);
							if (sqrDst < radius * radius) 
							{
								return false;
							}
						}
					}
				}
				return true;
			}
			return false;
		}
	};
}
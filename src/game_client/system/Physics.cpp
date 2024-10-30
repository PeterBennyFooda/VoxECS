#include "game_client/system/Physics.h"
#include "game_client/GameScene.h"

namespace VoxelWasteland
{
	Physics::Physics(sf::RenderWindow* win, GameScene* scene) 
		: window(win), scene(scene)
	{
	}

	Physics::~Physics()
	{
	}

	/*
	* @brief Update physics
	* @param delta: Delta time
	*/
	void Physics::Update(float delta)
	{
		auto& entities = scene->GetEntities();
		for (auto e : entities)
		{
			CTransform* transform = scene->GetComponent<CTransform>(e);
			CPhysics* physics = scene->GetComponent<CPhysics>(e);

			if (transform && physics)
			{
				if(!CREATIVE_MODE)
					physics->acceleration.y += GRAVITY * physics->mass * delta;

				// ========== Motion physics ==========
				float accX = physics->acceleration.x;
				float accY = physics->acceleration.y;
				float accZ = physics->acceleration.z;

				glm::vec3 finalPos = transform->position;

				// Check multiple times for collision between current position and new position
				for (int i = 4; i > 0; --i)
				{
					float xDelta = accX * delta / i;
					float yDelta = accY * delta / i;
					float zDelta = accZ * delta / i;

					glm::vec3 posXDir = transform->position + glm::vec3(xDelta, 0, 0);
					glm::vec3 posYDir = transform->position + glm::vec3(0, yDelta, 0);
					glm::vec3 posZDir = transform->position + glm::vec3(0, 0, zDelta);

					glm::vec3 box = glm::vec3(physics->halfWidth, physics->halfHeight, physics->halfDepth);

					if (!CheckCollision(posXDir, box))
					{
						finalPos.x = posXDir.x;
					}

					if (!CheckCollision(posYDir, box))
					{
						finalPos.y = posYDir.y;
					}
					else
					{
						physics->acceleration.y = 0;
					}

					if (!CheckCollision(posZDir, box))
					{
						finalPos.z = posZDir.z;
					}
				}

				transform->position = finalPos;

				// ========== Raycast ==========
				if (physics->raycast.active)
				{
					physics->raycast.hit = false;
				
					/*for (auto& pair : physics->raycast.hitPoints) // keeping the chunk would mess up the hit points order
					{
						pair.second.clear();
					}*/
					physics->raycast.hitPoints.clear(); // clear all hit points

					int count = physics->raycast.distance / (BLOCK_SIZE * 0.2f);
					for (int i = 1; i<=count; ++i)
					{
						float lerp = i / (float)count;
						float lerp_dist = lerp * physics->raycast.distance;

						float x = physics->raycast.origin.x + physics->raycast.direction.x * lerp_dist;
						float y = physics->raycast.origin.y + physics->raycast.direction.y * lerp_dist;
						float z = physics->raycast.origin.z + physics->raycast.direction.z * lerp_dist;

						glm::vec3 target = glm::vec3(x, y, z);
						glm::vec3 box = glm::vec3(physics->raycast.radius, physics->raycast.radius, physics->raycast.radius);

						Core::Chunk* chunk = nullptr;
						if (CheckCollision(target, box, chunk))
						{
							physics->raycast.hitPoints[chunk].push_back(target);
							physics->raycast.hit = true;
						}
					}
					
					if (physics->raycast.hitPoints.size() <= 0)
					{
						physics->raycast.hit = false;
					}
				}
			}
		}
	}

	/*
	* @brief Check collision
	* @param pos: Position to check collision
	* @param box: Box size
	* @param chunk: Chunk that contains the block
	* @return bool: True if collision, false otherwise
	*/
	bool Physics::CheckCollision(const glm::vec3& pos, const glm::vec3& box)
	{
		Core::Chunk* chunk = nullptr;
		return CheckCollision(pos, box, chunk);
	}
	bool Physics::CheckCollision(const glm::vec3& pos, const glm::vec3& box, Core::Chunk*& chunk)
	{
		Entity world = scene->GetWorld();
		CMap* map = scene->GetComponent<CMap>(world);
		if (map)
		{
			AABB self;
			self.left = pos.x - box.x;
			self.right = pos.x + box.x;
			self.bottom = pos.y - box.y;
			self.top = pos.y + box.y;
			self.back = pos.z - box.z;
			self.front = pos.z + box.z;

			glm::vec3 blockPos = glm::vec3(0);

			for (auto& ce : map->chunkEntities)
			{
				CChunk* cc = scene->GetComponent<CChunk>(ce);
				chunk = cc->chunk;

				if(!cc->active)
					continue;
					
				if (chunk->CheckBlockExists(self.left, self.bottom, self.back, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}

				if (chunk->CheckBlockExists(self.right, self.bottom, self.back, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}

				if (chunk->CheckBlockExists(self.left, self.top, self.back, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}

				if (chunk->CheckBlockExists(self.right, self.top, self.back, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}

				if (chunk->CheckBlockExists(self.left, self.bottom, self.front, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}

				if (chunk->CheckBlockExists(self.right, self.bottom, self.front, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}

				if (chunk->CheckBlockExists(self.left, self.top, self.front, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}

				if (chunk->CheckBlockExists(self.right, self.top, self.front, blockPos))
				{
					AABB block;
					block.left = blockPos.x - BLOCK_SIZE;
					block.right = blockPos.x + BLOCK_SIZE;
					block.bottom = blockPos.y - BLOCK_SIZE;
					block.top = blockPos.y + BLOCK_SIZE;
					block.back = blockPos.z - BLOCK_SIZE;
					block.front = blockPos.z + BLOCK_SIZE;

					if (Intersect(self, block))
						return true;
				}
			}
		}
		return false;
	}

	/*
	* @brief Check intersection between two AABBs
	* @param a: AABB a
	* @param b: AABB b
	* @return bool: True if intersect, false otherwise
	*/
	bool Physics::Intersect(const AABB& a, const AABB& b)
	{
		return (a.right >= b.left && a.left <= b.right) &&
			(a.top >= b.bottom && a.bottom <= b.top) &&
			(a.front >= b.back && a.back <= b.front);
	}
}
#pragma once

namespace VoxelWasteland
{
	namespace Core { class Chunk; }
	class GameScene;

	class Physics
	{
	public:
		struct AABB
		{
			float left;
			float right;
			float bottom;
			float top;
			float back;
			float front;
		};

		Physics(sf::RenderWindow* win, GameScene* scene);
		~Physics();

		void Update(float delta);

	private:
		Physics() = default;

		bool CheckCollision(const glm::vec3& pos, const glm::vec3& box);
		bool CheckCollision(const glm::vec3& pos, const glm::vec3& box, Core::Chunk*& chunk);
		bool Intersect(const AABB& a, const AABB& b);

		GameScene* scene;
		sf::RenderWindow* window;
	};
}
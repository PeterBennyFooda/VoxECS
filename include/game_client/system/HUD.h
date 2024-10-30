#pragma once

namespace VoxelWasteland
{
	class GameScene;

	class HUD
	{
	public:
		HUD(sf::RenderWindow* win, GameScene* scene);
		~HUD();

		void Init();
		void Update(float delta);

	private:
		HUD() = default;

		void CreateHUD();

		void CreateCrosshair(
			const int& x,
			const int& y,
			std::vector<Vertex2D>& vertices,
			std::vector<unsigned int>& indices
		);

		sf::RenderWindow* window;
		GameScene* scene;
	};
}
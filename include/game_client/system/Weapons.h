#pragma once

namespace VoxelWasteland
{
	class GameScene;

	class Weapons
	{
	public:
		Weapons(sf::RenderWindow* win, GameScene* scene);
		~Weapons();

		void Init();
		void Update(float delta);

	private:
		Weapons() = default;

		size_t CreateWeapon(size_t entity);

		void CreateWeaponData(
			const int& x,
			const int& y,
			const int& z,
			std::vector<Vertex>& vertices,
			std::vector<unsigned int>& indices
		);

		GameScene* scene;
		sf::RenderWindow* window;

		size_t playerWeapon;
		std::vector<size_t> weapons;
	};
}
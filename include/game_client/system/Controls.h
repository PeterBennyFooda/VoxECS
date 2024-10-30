#pragma once

namespace VoxelWasteland
{
	class GameScene;

	class Controls
	{
	public:
		Controls(sf::RenderWindow* win, GameScene* scene);
		~Controls();

		bool Update(float delta);

	private:
		Controls() = default;

		void CameraControl(float delta, size_t e);
		void MotionControl(float delta, size_t e);
		void InteractionControl(float delta, size_t e);

		GameScene* scene;
		sf::RenderWindow* window;

		// delay startup
		float startDelayTimer = 0.0f;
		bool jumped = false;
		bool canSprint = true;
		bool paused = false;

		bool LClicked = false;
		bool RClicked = false;

		bool sphereMode = false;
		int testPlaceID = 0;
	};
}


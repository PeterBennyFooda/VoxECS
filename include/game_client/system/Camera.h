#pragma once

namespace VoxelWasteland
{
	class GameScene;
	struct Plane;
	struct Frustum;
	struct CTransform;

	class Camera
	{
	public:
		Camera(sf::RenderWindow* win, GameScene* scene);
		~Camera();

		void Update(float delta);

	private:
		Camera() = default;

		bool IsOnFrustum(const Frustum& camFrustum, const glm::vec3& pos) const;
		bool IsOnOrForwardPlane(const Plane& plane, const glm::vec3& pos) const;

		Frustum Camera::CreateFrustumFromCamera(CTransform& cam, float aspect, float fovY, float zNear, float zFar);

		GameScene* scene;
		sf::RenderWindow* window;
	};
}
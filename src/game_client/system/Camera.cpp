#include "game_client/system/Camera.h"
#include "game_client/GameScene.h"

namespace VoxelWasteland
{
	Camera::Camera(sf::RenderWindow* win, GameScene* scene) :
		window(win), scene(scene)
	{
	}

	Camera::~Camera()
	{
	}

	/*
	* @brief Update the camera
	* @param delta: time since last frame
	*/
	void Camera::Update(float delta)
	{
		Entity player = scene->GetPlayer();
		CTransform* cameraTransform = scene->GetComponent<CTransform>(player);
		CCamera* camera = scene->GetComponent<CCamera>(player);

		if (camera && cameraTransform)
		{
			float aspect = (float)window->getSize().x / (float)window->getSize().y;
			camera->cameraFrustum = CreateFrustumFromCamera(*cameraTransform, aspect, camera->initialFoV, CAMERA_NEAR, CAMERA_FAR);
		}
		
		auto& entities = scene->GetEntities();
		float maxHeight = CHUNK_HEIGHT * (BLOCK_SIZE * 2);
		float halfChunkSize = CHUNK_SIZE * (BLOCK_SIZE * 2) * 0.5f;

		for (auto e : entities)
		{
			CTransform* transform = scene->GetComponent<CTransform>(e);
			CRenderable* renderable = scene->GetComponent<CRenderable>(e);
			CChunk* cchunk = scene->GetComponent<CChunk>(e);

			if (transform && renderable && camera)
			{
				if (cchunk && cameraTransform)
				{
					renderable->shoudRender = false;

					for (float y = 0; y < maxHeight; y += (BLOCK_SIZE * 2))
					{
						if (IsOnFrustum(camera->cameraFrustum, glm::vec3(transform->position.x, y, transform->position.z)) ||
							IsOnFrustum(camera->cameraFrustum, glm::vec3(transform->position.x + halfChunkSize, y, transform->position.z + halfChunkSize)) ||
							IsOnFrustum(camera->cameraFrustum, glm::vec3(transform->position.x - halfChunkSize, y, transform->position.z - halfChunkSize)) ||
							IsOnFrustum(camera->cameraFrustum, glm::vec3(transform->position.x - halfChunkSize, y, transform->position.z + halfChunkSize)) ||
							IsOnFrustum(camera->cameraFrustum, glm::vec3(transform->position.x + halfChunkSize, y, transform->position.x - halfChunkSize)))
						{
							renderable->shoudRender = true;
							break;
						}
					}
				}
				else
				{
					renderable->shoudRender = IsOnFrustum(camera->cameraFrustum, transform->position);
				}
			}
		}
	}

	/*
	* @brief Check if transform is on frustum
	* @param camFrustum: Camera frustum
	* @param transform: Transform to check
	* @return true if transform is on frustum. false if not.
	*/
	bool Camera::IsOnFrustum(const Frustum& camFrustum, const glm::vec3& position) const
	{
		// Scaled orientation
		return (IsOnOrForwardPlane(camFrustum.leftFace, position) &&
			IsOnOrForwardPlane(camFrustum.rightFace, position) &&
			IsOnOrForwardPlane(camFrustum.topFace, position) &&
			IsOnOrForwardPlane(camFrustum.bottomFace, position) &&
			IsOnOrForwardPlane(camFrustum.nearFace, position) &&
			IsOnOrForwardPlane(camFrustum.farFace, position));
	}

	/*
	* @brief Check if transform is on or forward plane
	* @param plane: Plane to check
	* @return true if transform is on or forward plane. false if not.
	*/
	bool Camera::IsOnOrForwardPlane(const Plane& plane, const glm::vec3& pos) const
	{
		// Compute the projection interval radius of bounding volume b onto L(t) = b.c + t * p.n
		float extent = (CHUNK_SIZE * (BLOCK_SIZE * 2));
		float r = extent * (std::abs(plane.normal.x) + std::abs(plane.normal.y) + std::abs(plane.normal.z));
		float distToPlane = plane.GetSignedDistanceToPlane(pos);

		// all planes are pointing inwards of the frustum
		// so if the point is on or forward the plane, it is inside the frustum
		// on the other hand, if the point is behind the plane, it is outside the frustum
		return -r <= distToPlane; 
	}

	/*
	* @brief Create frustum from camera
	* @param cam: Camera transform
	* @param aspect: Aspect ratio
	* @param fovY: Field of view in y axis
	* @param zNear: Near plane
	* @param zFar: Far plane
	* @return Frustum created from camera
	*/
	Frustum Camera::CreateFrustumFromCamera(CTransform& cam, float aspect, float fovY, float zNear, float zFar)
	{
		Frustum frustum;
		float halfVSide = zFar * tanf(fovY * .5f);
		float halfHSide = halfVSide * aspect;
		glm::vec3 frontMultFar = zFar * cam.GetForward();

		// Note: all normals are pointing inwards
		frustum.nearFace = { cam.position + zNear * cam.GetForward(), cam.GetForward()};
		frustum.farFace = { cam.position + frontMultFar, cam.GetBackward() };
		frustum.rightFace = { cam.position, glm::cross(frontMultFar - cam.GetRight() * halfHSide, cam.GetUp()) };
		frustum.leftFace = { cam.position, glm::cross(cam.GetUp(), frontMultFar + cam.GetRight() * halfHSide)};
		frustum.topFace = { cam.position, glm::cross(cam.GetRight(), frontMultFar - cam.GetUp() * halfVSide)};
		frustum.bottomFace = { cam.position, glm::cross(frontMultFar + cam.GetUp() * halfVSide, cam.GetRight()) };
		return frustum;
	}
}
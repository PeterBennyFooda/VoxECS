#include "game_client/system/Controls.h"
#include "game_client/GameScene.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace VoxelWasteland
{
	Controls::Controls(sf::RenderWindow* win, GameScene* scene) : window(win), scene(scene)
	{
		auto& entities = scene->GetEntities();
		for (auto e : entities)
		{
			CTransform* transform = scene->GetComponent<CTransform>(e);
			CControllable* controllable = scene->GetComponent<CControllable>(e);
			CCamera* camera = scene->GetComponent<CCamera>(e);

			if (transform && controllable && camera)
			{
				controllable->lastMouseX = window->getSize().x / 2;
				controllable->lastMouseY = window->getSize().y / 2;

				transform->position = glm::vec3(10, 50, 10);
				camera->verticalAngle = 0.0f;
				camera->horizontalAngle = 0.0f;
			}
		}
	}

	Controls::~Controls()
	{
	}

	/*
	* @brief Update controls
	* @param delta Delta time
	* @return true if successful. false if failed.
	*/
	bool Controls::Update(float delta)
	{
		if (startDelayTimer <= 1.0f)
		{
			startDelayTimer += delta;
			return false;
		}

		auto& entities = scene->GetEntities();
		for (auto e : entities)
		{
			CTransform* transform = scene->GetComponent<CTransform>(e);
			CControllable* controllable = scene->GetComponent<CControllable>(e);
			CCamera* camera = scene->GetComponent<CCamera>(e);
			CPhysics* physics = scene->GetComponent<CPhysics>(e);

			if (transform && controllable && camera && physics)
			{
				float width = window->getSize().x;
				float height = window->getSize().y;

				CameraControl(delta, e);
				MotionControl(delta, e);
				InteractionControl(delta, e);
			}
		}

		return true;
	}

	void Controls::CameraControl(float delta, size_t e)
	{
		if (paused)
			return;

		CTransform* transform = scene->GetComponent<CTransform>(e);
		CControllable* controllable = scene->GetComponent<CControllable>(e);
		CCamera* camera = scene->GetComponent<CCamera>(e);

		double xPos, yPos;
		sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
		xPos = mousePos.x;
		yPos = mousePos.y;

		float width = window->getSize().x;
		float height = window->getSize().y;

		float deltaH = controllable->cameraSpeed * (controllable->lastMouseX - xPos) * delta;
		float deltaV = controllable->cameraSpeed * (controllable->lastMouseY - yPos) * delta;

		// Mouse control
		{
			if (xPos < 0 + (width*0.1f))
				xPos = 0 + (width * 0.1f);
			else if (xPos > width - (width * 0.1f))
				xPos = width - (width * 0.1f);

			if (yPos < 0 + (height * 0.1f))
				yPos = 0 + (height * 0.1f);
			else if (yPos > height - (height * 0.1f))
				yPos = height - (height * 0.1f);

			sf::Mouse::setPosition(sf::Vector2i(xPos, yPos), *window);

			controllable->lastMouseX = xPos;
			controllable->lastMouseY = yPos;

			camera->horizontalAngle += deltaH;
			camera->verticalAngle += deltaV;

			if (camera->verticalAngle > CAMERA_MAX_PITCH)
				camera->verticalAngle = CAMERA_MAX_PITCH;
			else if (camera->verticalAngle < CAMERA_MIN_PITCH)
				camera->verticalAngle = CAMERA_MIN_PITCH;
		}

		float FoV = camera->initialFoV;

		glm::vec3 direction
		(
			cos(camera->verticalAngle) * sin(camera->horizontalAngle),
			sin(camera->verticalAngle),
			cos(camera->verticalAngle) * cos(camera->horizontalAngle)
		);

		// Pi/2 radians = 90 degrees
		float rightAngleOffset = 3.14f / 2.0f;
		glm::vec3 right
		(
			sin(camera->horizontalAngle - rightAngleOffset),
			0,
			cos(camera->horizontalAngle - rightAngleOffset)
		);

		// Up vector
		glm::vec3 up = glm::cross(right, direction);

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		camera->projectionMatrix = glm::perspective(glm::radians(FoV), width / height, CAMERA_NEAR, CAMERA_FAR);

		// Camera matrix
		camera->viewMatrix = glm::lookAt
		(
			transform->position,				// Camera is here
			transform->position + direction,	// and looks here : at the same position, plus "direction"
			up									// Head is up (set to 0,-1,0 to look upside-down)
		);

		float rotX = glm::degrees(camera->verticalAngle);
		float rotY = glm::degrees(camera->horizontalAngle) + 180.0f;
		float rotZ = 0.0f;
		transform->rotation = glm::vec3(rotX, rotY, rotZ);
	}

	void Controls::MotionControl(float delta, size_t e)
	{
		CControllable* controllable = scene->GetComponent<CControllable>(e);
		CCamera* camera = scene->GetComponent<CCamera>(e);
		CPhysics* physics = scene->GetComponent<CPhysics>(e);

		glm::vec3 direction
		(
			cos(camera->verticalAngle) * sin(camera->horizontalAngle),
			sin(camera->verticalAngle),
			cos(camera->verticalAngle) * cos(camera->horizontalAngle)
		);

		glm::vec3 forward
		(
			sin(camera->horizontalAngle),
			0,
			cos(camera->horizontalAngle)
		);

		// Pi/2 radians = 90 degrees
		float rightAngleOffset = 3.14f / 2.0f;
		glm::vec3 right
		(
			sin(camera->horizontalAngle - rightAngleOffset),
			0,
			cos(camera->horizontalAngle - rightAngleOffset)
		);

		// Up vector
		glm::vec3 up = glm::cross(right, direction);

		if(!paused)
		{
			canSprint = true;

			if (CREATIVE_MODE)
				physics->acceleration = glm::vec3(0, 0, 0);
			else
				physics->acceleration = glm::vec3(0, physics->acceleration.y, 0);

			// ============ Moving ============
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			{
				//transform->position = transform->position - right * delta * controllable->speed;
				physics->acceleration += -right * controllable->speed;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			{
				//transform->position = transform->position + right * delta * controllable->speed;
				physics->acceleration += right * controllable->speed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			{
				//transform->position = transform->position - direction * delta * controllable->speed;
				physics->acceleration += -forward * controllable->speed;
				canSprint = false;
			}
			else
			{
				canSprint = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			{
				//transform->position = transform->position + direction * delta * controllable->speed;
				physics->acceleration += forward * controllable->speed;
			}

			// ============ Jumping ============
			if (CREATIVE_MODE)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
				{
					glm::vec3 jumpDir = glm::vec3(0, 1, 0);
					//transform->position = transform->position + jumpDir * delta * controllable->speed;
					physics->acceleration += jumpDir * controllable->jumpForce;
				}
			}
			else
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !jumped)
				{
					glm::vec3 jumpDir = glm::vec3(0, 1, 0);
					//transform->position = transform->position + jumpDir * delta * controllable->speed;
					physics->acceleration += jumpDir * controllable->jumpForce;

					jumped = true;
				}
				else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
				{
					jumped = false;
				}
			}

			// ================= Sprinting =================
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && canSprint)
			{
				physics->acceleration.x *= DEFAULT_SPRINT_MOD;
				physics->acceleration.z *= DEFAULT_SPRINT_MOD;
			}

			// ================= Flying =================
			if (CREATIVE_MODE && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
			{
				physics->acceleration.y = -controllable->jumpForce;
			}
		}

		// ================= Pausing =================
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
		{
			paused = !paused;
		}
	}

	/*
	* @brief Interaction control
	* @param delta Delta time
	* @param e Entity ID
	*/
	void Controls::InteractionControl(float delta, size_t e)
	{
		if (paused)
			return;

		// Update player interaction
		CTransform* transform = scene->GetComponent<CTransform>(e);
		CPhysics* physics = scene->GetComponent<CPhysics>(e);
		CCamera* camera = scene->GetComponent<CCamera>(e);
		if (transform && physics)
		{
			// raycast
			glm::vec3 direction
			(
				cos(camera->verticalAngle) * sin(camera->horizontalAngle),
				sin(camera->verticalAngle),
				cos(camera->verticalAngle) * cos(camera->horizontalAngle)
			);

			physics->raycast.origin = transform->position;
			physics->raycast.direction = direction;
			physics->raycast.distance = 10.0f;
			physics->raycast.radius = 0.0f;
			physics->raycast.active = true;

			// Left mouse button
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !LClicked)
			{
				LClicked = true;
				if (physics->raycast.hit)
				{
					for (auto& hit : physics->raycast.hitPoints)
					{
						if (!hit.second.empty())
						{
							if (sphereMode)
							{
								// remove a sphere of blocks
								float radius = BLOCK_SIZE * 10.0f;
								int rSquared = radius * radius;
								std::vector<glm::vec3> blocks;
								std::vector<unsigned int> ids;

								glm::vec3 startPoint = hit.second[0] + glm::vec3(0.0f, radius, 0.0f);

								for (int x = -radius; x <= radius; ++x) {
									for (int y = -radius; y <= radius; ++y) {
										for (int z = -radius; z <= radius; ++z) {
											glm::vec3 offset(x, y, z);

											// Check if the voxel is within the sphere
											if (x * x + y * y + z * z <= rSquared) {
												blocks.push_back(startPoint + offset);
												ids.push_back(testPlaceID);
											}
										}
									}
								}

								hit.first->TryAddBlocks(hit.second[0], blocks, ids);
							}
							else
							{
								// add a block adjacent to the hit point
								hit.first->TryAddBlock(hit.second[0], testPlaceID);
							}
							break;
						}
					}
				}
			}
			else if(!sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				LClicked = false;
			}

			// Right mouse button
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !RClicked)
			{
				RClicked = true;
				if (physics->raycast.hit)
				{
					for (auto& hit : physics->raycast.hitPoints)
					{
						if (!hit.second.empty())
						{
							if (sphereMode)
							{
								// remove a sphere of blocks
								float radius = BLOCK_SIZE * 10.0f;
								int rSquared = radius * radius;
								std::vector<glm::vec3> blocks;

								for (int x = -radius; x <= radius; ++x) {
									for (int y = -radius; y <= radius; ++y) {
										for (int z = -radius; z <= radius; ++z) {
											glm::vec3 offset(x, y, z);

											// Check if the voxel is within the sphere
											if (x * x + y * y + z * z <= rSquared) {
												blocks.push_back(hit.second[0] + offset);
											}
										}
									}
								}

								hit.first->RemoveBlocks(blocks);
							}
							else
							{
								// remove a block
								hit.first->RemoveBlock(hit.second[0]);
							}
							break;
						}
					}
				}
			}
			else if (!sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				RClicked = false;
			}

			// place mode button
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
			{
				sphereMode = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))
			{
				sphereMode = false;
			}

			// test place id
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1))
			{
				testPlaceID = 0;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2))
			{
				testPlaceID = 1;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3))
			{
				testPlaceID = 2;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4))
			{
				testPlaceID = 3;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5))
			{
				testPlaceID = 4;
			}
		}
	}
}

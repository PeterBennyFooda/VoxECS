#pragma once

////////////////////////////////////////////////////////////
// This file defines all the components.
// We can extend this list by adding custom components.
////////////////////////////////////////////////////////////

namespace VoxelWasteland
{
	namespace Core { class Chunk; }

	/*
	 * (1) CTransform
	 *
	 * This Component contains position, rotation
	 * and scale of an entity.
	 *
	 * We can use this compoenet to grant an entity
	 * the basic information to interact with the world
	 * and other entities.
	 */
	struct CTransform
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // Euler angles

	private:
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		glm::mat4 GetLocalModelMatrix() const
		{
			const glm::mat4 transformX = 
				glm::rotate(glm::mat4(1.0f),
				glm::radians(rotation.x),
				glm::vec3(1.0f, 0.0f, 0.0f));
			const glm::mat4 transformY = 
				glm::rotate(glm::mat4(1.0f),
				glm::radians(rotation.y),
				glm::vec3(0.0f, 1.0f, 0.0f));
			const glm::mat4 transformZ = 
				glm::rotate(glm::mat4(1.0f),
				glm::radians(rotation.z),
				glm::vec3(0.0f, 0.0f, 1.0f));

			// Y * X * Z
			const glm::mat4 roationMatrix = transformY * transformX * transformZ;

			// translation * rotation * scale (also know as TRS matrix)
			return glm::translate(glm::mat4(1.0f), position) * roationMatrix * glm::scale(glm::mat4(1.0f), scale);
		}

	public:
		glm::mat4& GetModelMatrix()
		{
			modelMatrix = GetLocalModelMatrix();
			return modelMatrix;
		}

		glm::mat4& GetModelMatrix(const glm::mat4& parentGlobalModelMatrix)
		{
			modelMatrix = parentGlobalModelMatrix * GetLocalModelMatrix();
			return modelMatrix;
		}

		glm::vec3 GetRight()
		{
			modelMatrix = GetLocalModelMatrix();
			return modelMatrix[0];
		}

		glm::vec3 GetUp()
		{
			modelMatrix = GetLocalModelMatrix();
			return modelMatrix[1];
		}

		glm::vec3 GetBackward()
		{
			modelMatrix = GetLocalModelMatrix();
			return modelMatrix[2];
		}

		glm::vec3 GetForward()
		{
			modelMatrix = GetLocalModelMatrix();
			return -modelMatrix[2];
		}
	};

	/*
	*  (2) CRectTransform
	* 
	*  This Component is for 2D UI elements.
	*/
	struct CRectTransform
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // Euler angles

		float width = 1.0f;
		float height = 1.0f;
	};

	/*
	 * (3) CRenderable
	 *
	 * This Component contains the information
	 * to render an entity.
	 *
	 * We can use this component to grant an entity
	 * the ability to be rendered on the screen.
	 */
	namespace Core 
	{ 
		class CustomShader;
		class Mesh;
		class Mesh2D;
	}
	struct CRenderable
	{
		std::vector<Texture> texture;
		std::vector<std::string> texturePath;

		Core::CustomShader* shader;
		std::string shaderPath = "";

		Core::Mesh* mesh;
		Core::Mesh2D* mesh2D;

		bool shoudRender = true;
	};

	/*
	* (4) CCamera
	* 
	* This Component contains the information
	* to render the scene from a camera's perspective.
	* 
	* We can use this component to grant an entity
	* the ability to be a camera.
	*/
	struct Plane
	{
		glm::vec3 normal = { 0.f, 1.f, 0.f };   // unit vector
		float     distance = 0.f;				// Distance with origin

		Plane() = default;
		Plane(const glm::vec3& p1, const glm::vec3& norm)
			: normal(glm::normalize(norm)),
			distance(glm::dot(normal, p1))
		{}

		float GetSignedDistanceToPlane(const glm::vec3& point) const
		{
			return glm::dot(normal, point) - distance;
		}
	};
	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;
	};
	struct CCamera
	{
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		Frustum cameraFrustum;

		float horizontalAngle = 3.14f;
		float verticalAngle = 0.0f;
		float initialFoV = CAMERA_FOV;

		unsigned int renderDistance = MAP_RENDER_DISTANCE;
	};

	/*
	* (5) CPhysics
	* 
	* This Component makes an entity affected by physics.
	*
	* We can use this compoenet to give an entity a
	* basic collider and other physics properties.
	*/
	struct Raycast
	{
		glm::vec3 origin;
		glm::vec3 direction;
		float distance;
		float radius;

		std::unordered_map<Core::Chunk*, std::vector<glm::vec3>> hitPoints;
		bool hit = false;
		bool active = false;
	};
	struct CPhysics
	{
		glm::vec3 acceleration;
		float halfWidth, halfHeight, halfDepth;
		float mass;

		Raycast raycast;
	};

	/*
	* (6) CChunk
	* 
	* This Component contains chunk information.
	* 
	*/
	struct CChunk
	{
		Core::Chunk* chunk = nullptr;
		bool active = false;
	};

	/*
	*  (7) CMap
	*
	* This Component contains map information.
	* 
	*/
	struct CMap
	{
		unsigned int size = MAP_SIZE;
		std::vector<size_t> chunkEntities;
	};

	/*
	* (8) CCharacter
	* 
	* This Component contains the information
	* about a character.
	* 
	* We can use this component to give an entity
	* unique character traits.
	*/
	struct CCharacter
	{
		int id = -1;
		std::string name = "Unknown Entity";

		int health = 100;
		bool isAlive = true;
	};

	/*
	* (9) CControllable
	* 
	* This Component has player input information.
	*/
	struct CControllable
	{
		float speed = DEFAULT_MOVE_SPEED;
		float jumpForce = DEFAULT_JUMP_FORCE;
		float cameraSpeed = CAMERA_LOOK_SPEED;
		float lastMouseX = 0;
		float lastMouseY = 0;
	};

	/*
	*  (10) CWeapon
	* 
	* This Component contains weapon information.
	*/
	struct CWeapon
	{
		int id = -1;
		std::string name = "Unknown Weapon";

		int maxAmmo = 10;
		int currentAmmo = 0;
		float reloadTime = 1.0f;

		float fireRate = 0.5f;
		int damage = 10;
	};
}
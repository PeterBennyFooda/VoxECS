#include "game_client/system/Weapons.h"
#include "game_client/GameScene.h"
#include "core/render/Mesh.h"

namespace VoxelWasteland
{
	Weapons::Weapons(sf::RenderWindow* win, GameScene* scene):
		window(win), scene(scene)
	{
		Init();
	}

	Weapons::~Weapons()
	{
	}

	/*
	* @brief Initialize weapons
	*/
	void Weapons::Init()
	{
		playerWeapon = CreateWeapon(scene->GetPlayer());
	}

	/*
	* @brief Update weapons
	*/
	void Weapons::Update(float delta)
	{
		CTransform* transform = scene->GetComponent<CTransform>(playerWeapon);
		transform->position = glm::vec3(0.75f, -0.55f, -1.5f);
		transform->rotation = glm::vec3(25.0f, 5.0f, -10.0f);
		transform->scale = glm::vec3(0.75f, 0.75f, 0.75f);
	}

	/*
	* @brief Create weapon for entity
	* @param entity: Entity to create weapon for
	* @return Weapon entity id
	*/
	size_t Weapons::CreateWeapon(size_t entity)
	{
		// retrieve weapon component
		size_t weaponEntity = scene->CreateEntity<CTransform, CWeapon, CRenderable>();
		weapons.push_back(weaponEntity);

		CTransform* transform = scene->GetComponent<CTransform>(weaponEntity);
		CWeapon* weapon = scene->GetComponent<CWeapon>(weaponEntity);
		CRenderable* renderable = scene->GetComponent<CRenderable>(weaponEntity);

		// set weapon transform
		glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
		transform->position = offset;

		// set weapon data
		weapon->id = 0;
		weapon->name = "Weapon";

		// set weapon renderable data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		CreateWeaponData(
			0,
			0,
			0,
			vertices,
			indices
		);

		renderable->texturePath = std::vector<std::string>{ "asset/block/stone.png" };
		renderable->shaderPath = "shader/weapon";
		std::string vertPath = renderable->shaderPath + ".vert";
		std::string fragPath = renderable->shaderPath + ".frag";
		renderable->shader = new Core::CustomShader(vertPath.c_str(), fragPath.c_str());
		unsigned int texID = Core::ResourceHelper::LoadTextureArray(renderable->texturePath, 16, 16);
		Texture tex = { texID, "texture_diffuse" };
		renderable->texture = std::vector<Texture>{ tex };

		renderable->mesh = new Core::Mesh(vertices, indices, renderable->texture);

		return weaponEntity;
	}

	/*
	* @brief Create weapon data
	* @param x: x position
	* @param y: y position
	* @param z: z position
	* @param vertices: Vertices
	*/
	void Weapons::CreateWeaponData(
		const int& x, 
		const int& y, 
		const int& z, 
		std::vector<Vertex>& vertices,
		std::vector<unsigned int>& indices)
	{
		const float handWidth = BLOCK_SIZE * 0.5f;
		const float handHeight = BLOCK_SIZE * 0.4f;
		const float handDepth = BLOCK_SIZE * 1.25f;

		// 8 vertices of a cube
		glm::vec3 p1 = glm::vec3(x - handWidth, y - handHeight, z + handDepth); // z+, bottom left
		glm::vec3 p2 = glm::vec3(x + handWidth, y - handHeight, z + handDepth); // z+, bottom right
		glm::vec3 p3 = glm::vec3(x + handWidth, y + handHeight, z + handDepth); // z+, top right
		glm::vec3 p4 = glm::vec3(x - handWidth, y + handHeight, z + handDepth); // z+, top left
		glm::vec3 p5 = glm::vec3(x + handWidth, y - handHeight, z - handDepth); // z-, bottom right
		glm::vec3 p6 = glm::vec3(x - handWidth, y - handHeight, z - handDepth); // z-, bottom left
		glm::vec3 p7 = glm::vec3(x - handWidth, y + handHeight, z - handDepth); // z-, top left
		glm::vec3 p8 = glm::vec3(x + handWidth, y + handHeight, z - handDepth); // z-, top right

		Vertex v;
		glm::vec3 n = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec2 uv = glm::vec2(0.0f, 0.0f);

		// =====  Front face =====
		{
			n = glm::vec3(0.0f, 0.0f, 1.0f);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p1, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p2, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p3, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p4, n, uv };
			vertices.push_back(v);

			// create indices for the face (2 triangles), front: p1, p2, p3, p1, p3, p4
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 3);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 1);
		}

		// =====  Back face =====
		{
			n = glm::vec3(0.0f, 0.0f, -1.0f);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p5, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p6, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p7, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p8, n, uv };
			vertices.push_back(v);

			// back: p5, p6, p7, p5, p7, p8
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 3);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 1);
		}

		// ===== Right face ===== 
		{
			n = glm::vec3(1.0f, 0.0f, 0.0f);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p2, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p5, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p8, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p3, n, uv };
			vertices.push_back(v);

			// right: p2, p5, p8, p2, p8, p3
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 3);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 1);
		}

		// =====  Left face =====
		{
			n = glm::vec3(-1.0f, 0.0f, 0.0f);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p6, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p1, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p4, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p7, n, uv };
			vertices.push_back(v);

			// left: p6, p1, p4, p6, p4, p7
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 3);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 1);
		}

		//=====  Top face =====
		{
			n = glm::vec3(0.0f, 1.0f, 0.0f);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p4, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p3, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p8, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p7, n, uv };
			vertices.push_back(v);

			// top: p4, p3, p8, p4, p8, p7
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 3);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 1);
		}

		// =====  Bottom face ===== 
		{
			n = glm::vec3(0.0f, -1.0f, 0.0f);

			uv = glm::vec2(0.0f, 0.0f); // bottom left
			v = { p6, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 0.0f); // bottom right
			v = { p5, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(1.0f, 1.0f); // top right
			v = { p2, n, uv };
			vertices.push_back(v);

			uv = glm::vec2(0.0f, 1.0f); // top left
			v = { p1, n, uv };
			vertices.push_back(v);

			// bottom: p6, p5, p2, p6, p2, p1
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 3);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 4);
			indices.push_back(vertices.size() - 2);
			indices.push_back(vertices.size() - 1);
		}
	}
}
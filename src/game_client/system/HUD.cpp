#include "game_client/system/HUD.h"
#include "game_client/GameScene.h"
#include "core/render/Mesh2D.h"

namespace VoxelWasteland
{
	HUD::HUD(sf::RenderWindow* win, GameScene* scene) :
		window(win), scene(scene)
	{
		Init();
	}

	HUD::~HUD()
	{
	}

	/*
	* @brief Initialize HUD
	*/
	void HUD::Init()
	{
		CreateHUD();
	}

	/*
	* @brief Update HUD
	* @param delta: Time since the last update
	*/
	void HUD::Update(float delta)
	{
	}

	/*
	* @brief Create HUD
	*/
	void HUD::CreateHUD()
	{
		Entity hudEntity = scene->CreateEntity<CRectTransform, CRenderable>();

		CRectTransform* rectTransform = scene->GetComponent<CRectTransform>(hudEntity);
		CRenderable* renderable = scene->GetComponent<CRenderable>(hudEntity);

		// set transform
		glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
		rectTransform->position = offset;

		// set renderable data
		std::vector<Vertex2D> vertices;
		std::vector<unsigned int> indices;

		// Origin is at top left for orthographic projection
		// to center the UI, we need to calculate the center of the window
		float width = window->getSize().x / 2;
		float height = window->getSize().y / 2;
		CreateCrosshair(
			width,
			height,
			vertices,
			indices
		);

		renderable->texturePath = std::vector<std::string>{ "asset/crosshair.png" };
		renderable->shaderPath = "shader/tex2D";
		std::string vertPath = renderable->shaderPath + ".vert";
		std::string fragPath = renderable->shaderPath + ".frag";
		renderable->shader = new Core::CustomShader(vertPath.c_str(), fragPath.c_str());
		unsigned int texID = Core::ResourceHelper::LoadTexture(renderable->texturePath[0].c_str());
		Texture tex = { texID, "texture_diffuse" };
		renderable->texture = std::vector<Texture>{ tex };

		renderable->mesh2D = new Core::Mesh2D(vertices, indices, renderable->texture);
	}

	/*
	* @brief Create HUD
	* @param x: X position
	* @param y: Y position
	* @param vertices: Vertices
	* @param indices: Indices
	*/
	void HUD::CreateCrosshair(
		const int& x,
		const int& y,
		std::vector<Vertex2D>& vertices,
		std::vector<unsigned int>& indices
	)
	{
		// 4 vertices of a rectangle
		glm::vec3 p1 = glm::vec3(x - HUD_CROSSHAIR_SIZE, y - HUD_CROSSHAIR_SIZE, 0.0f);
		glm::vec3 p2 = glm::vec3(x + HUD_CROSSHAIR_SIZE, y - HUD_CROSSHAIR_SIZE, 0.0f);
		glm::vec3 p3 = glm::vec3(x + HUD_CROSSHAIR_SIZE, y + HUD_CROSSHAIR_SIZE, 0.0f);
		glm::vec3 p4 = glm::vec3(x - HUD_CROSSHAIR_SIZE, y + HUD_CROSSHAIR_SIZE, 0.0f);

		Vertex2D v;
		glm::vec2 uv = glm::vec2(0.0f, 0.0f);

		uv = glm::vec2(0.0f, 0.0f); // bottom left
		v = { p1, uv };
		vertices.push_back(v);

		uv = glm::vec2(1.0f, 0.0f); // bottom right
		v = { p2, uv };
		vertices.push_back(v);

		uv = glm::vec2(1.0f, 1.0f); // top right
		v = { p3, uv };
		vertices.push_back(v);

		uv = glm::vec2(0.0f, 1.0f); // top left
		v = { p4, uv };
		vertices.push_back(v);

		// create indices for the quad
		indices.push_back(vertices.size() - 4);
		indices.push_back(vertices.size() - 3);
		indices.push_back(vertices.size() - 2);
		indices.push_back(vertices.size() - 4);
		indices.push_back(vertices.size() - 2);
		indices.push_back(vertices.size() - 1);
	}
}
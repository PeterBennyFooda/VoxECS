#include "game_client/system/Renderer.h"
#include "game_client/GameScene.h"

#include "core/render/Mesh.h"
#include "core/render/Mesh2D.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/OpenGL.hpp>
#include <filesystem>

namespace VoxelWasteland
{
    Renderer::Renderer() :
        VAO(0), VBO(0), EBO(0), CBO(0),
        width(800), height(600)
    {
		window = nullptr;
    }

    Renderer::Renderer(sf::RenderWindow* win, GameScene* scene) :
        VAO(0), VBO(0), EBO(0), CBO(0),
		scene(scene)
    {
        window = win;
		width = window->getSize().x;
		height = window->getSize().y;

        Init();
    }

    Renderer::~Renderer()
    {
        delete shaderChunk;
    }

    /*
    * @brief initialize the renderer
    */
    void Renderer::Init()
    {
        // Initialize basic settings
        InitBasicSettings();

        // Set Attributes
        SetAtrributes();

        // Set Shaders
        SetShaders();
    }

    /*
    * @brief update the renderer
    * @param delta time
    */
    void Renderer::Update(float delta)
    {
        auto& entities = scene->GetEntities();
        for (auto e : entities)
        {
			CTransform* transform = scene->GetComponent<CTransform>(e);
            CCamera* camera = scene->GetComponent<CCamera>(e);
			CRenderable* renderable = scene->GetComponent<CRenderable>(e);

            if (camera)
            {
				cameraViewMatrix = camera->viewMatrix;
				cameraProjectionMatrix = camera->projectionMatrix;
                break;
            }        
        }
    }

    /*
    * @brief render the scene
    */
    void Renderer::Render()
    {
        RenderSkybox();

		RenderChunks();

        RenderWeapon();

        RenderUI();
    }

    /*
    * @brief initialize the basic settings
    */
    void Renderer::InitBasicSettings()
    {
        // Initialize glew
        glewExperimental = GL_TRUE;
        glewInit();

        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        // Cull triangles which normal is not towards the camera
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    /*
    * @brief set the attributes like VAO, VBO, EBO, etc.
    */
    void Renderer::SetAtrributes()
    {
		// Set up the skybox
        glGenVertexArrays(1, &Sky_VAO);
        glGenBuffers(1, &Sky_VBO);
        glBindVertexArray(Sky_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, Sky_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    /*
    * @brief set up the shaders
    */
    void Renderer::SetShaders()
    {
        // Shaders
        shaderChunk = new Core::CustomShader("shader/block.vert", "shader/block.frag");
		shaderSky = new Core::CustomShader("shader/sky.vert", "shader/sky.frag");

        std::vector<std::string> faces
        {
            "asset/skybox/Left+X.png",
            "asset/skybox/Right-X.png",
            "asset/skybox/Up+Y.png",
            "asset/skybox/Down-Y.png",
            "asset/skybox/Front+Z.png",
            "asset/skybox/Back-Z.png"
        };
        cubemapTexture = Core::ResourceHelper::LoadCubemap(faces);

        shaderSky->Use();
        shaderSky->SetInt("skybox", 0);
    }

    /*
	* @brief set camera properties
    */
    void Renderer::SetCamera(Core::CustomShader* shader, const size_t& e, const glm::mat4& mvp)
    {
        CTransform* transform = scene->GetComponent<CTransform>(e);
        if (transform)
        {
            shader->Use();

            // Send our transformation to the currently bound shader, in the "MVP" uniform
            shader->SetMat4("mvp", mvp);
            shader->SetMat4("model", transform->GetModelMatrix());
            shader->SetVec3("pointLight.position", transform->position);
        }
    }

    /*
	* @brief set lighting properties
    */
    void Renderer::SetLighting(Core::CustomShader* shader)
    {
        shader->Use();

        // point light properties
        shader->SetVec3("pointLight.ambient", { 0.25f, 0.25f, 0.25f });
        shader->SetVec3("pointLight.diffuse", { 0.25f, 0.25f, 0.25f });
        shader->SetVec3("pointLight.specular", { 1.0f, 1.0f, 1.0f });

        shader->SetFloat("pointLight.constant", DEFAULT_LIGHT_CONSTANT);
        shader->SetFloat("pointLight.linear", DEFAULT_PLAYER_LIGHT_LINEAR);
        shader->SetFloat("pointLight.quadratic", DEFAULT_PLAYER_LIGHT_QUADRATIC);

		// directional light properties
		shader->SetVec3("dirLight.direction", { -0.2f, -1.0f, -0.2f });
		shader->SetVec3("dirLight.ambient", { 0.325f, 0.325f, 0.325f });
		shader->SetVec3("dirLight.diffuse", { 0.8f, 0.8f, 0.8f });

        // material properties
        shader->SetVec3("material.specular", { 0.5f, 0.5f, 0.5f });
        shader->SetFloat("material.shininess", 64.0f);
    }

    /*
	* @brief render the chunks
    */
    void Renderer::RenderChunks()
    {
		Entity world = scene->GetWorld();

        CTransform* transform = scene->GetComponent<CTransform>(world);
        CMap* map = scene->GetComponent<CMap>(world);

        if (transform && map)
        {
            glm::mat4 mvp = cameraProjectionMatrix * cameraViewMatrix * glm::mat4(1.0f);

            SetLighting(shaderChunk);
            SetCamera(shaderChunk, world, mvp);

            for (auto& ce : map->chunkEntities)
            {
				CChunk* cc = scene->GetComponent<CChunk>(ce);
				CRenderable* cr = scene->GetComponent<CRenderable>(ce);

                if(cc&& cr && cr->shoudRender)
                    cc->chunk->Draw(shaderChunk);
            }
        }
    }

    /*
	* @brief render the skybox
    */
    void Renderer::RenderSkybox()
    {
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        shaderSky->Use();

        glm::mat4 view = glm::mat4(glm::mat3(cameraViewMatrix)); // remove translation from the view matrix
        glm::mat4 projection = cameraProjectionMatrix;
        shaderSky->SetMat4("view", view);
        shaderSky->SetMat4("projection", projection);

        // skybox cube
        glBindVertexArray(Sky_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS); // set depth function back to default 
    }

    /*
	* @brief render the weapon
    */
    void Renderer::RenderWeapon()
    {
        auto& entities = scene->GetEntities();
        for (auto e : entities)
        {
			CTransform* transform = scene->GetComponent<CTransform>(e);
			CWeapon* weapon = scene->GetComponent<CWeapon>(e);
			CRenderable* renderable = scene->GetComponent<CRenderable>(e);

            if (weapon && renderable)
            {
				glClear(GL_DEPTH_BUFFER_BIT); // clear depth buffer so weapon is rendered on top of everything

                if (renderable->shader && renderable->mesh)
                {
                    glm::mat4 modelToCameraSpace = glm::inverse(cameraViewMatrix) * transform->GetModelMatrix();
                    glm::mat4 mvp = cameraProjectionMatrix * cameraViewMatrix * modelToCameraSpace;

                    SetLighting(renderable->shader);
                    SetCamera(renderable->shader, e, mvp);
                    renderable->mesh->Draw(renderable->shader);
                }
            }
        }
    }

    /*
	* @brief render the UI
    */
    void Renderer::RenderUI()
    {
        auto& entities = scene->GetEntities();
        for (auto e : entities)
        {
			CRectTransform* rectTransform = scene->GetComponent<CRectTransform>(e);
			CRenderable* renderable = scene->GetComponent<CRenderable>(e);

            if (rectTransform && renderable)
            {
                if (renderable->shader && renderable->mesh2D)
                {
					glm::mat4 ortho = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f); // origin at top left
					glm::mat4 model = glm::mat4(1.0f);
					renderable->shader->Use();
					renderable->shader->SetMat4("model", model);
					renderable->shader->SetMat4("projection", ortho);
                    renderable->mesh2D->Draw(renderable->shader);
                }
            }
        }
    }
}

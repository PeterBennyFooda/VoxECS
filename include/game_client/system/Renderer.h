#pragma once

#include "Controls.h"
#include "core/render/CustomShader.h"
#include "core/world/ChunkManager.h"

namespace VoxelWasteland
{
    static constexpr GLfloat skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

	class GameScene;

    class Renderer
    {
    public:
        Renderer();
        Renderer(sf::RenderWindow* win, GameScene* scene);
        ~Renderer();

        void Update(float delta);
        void Render();

    private:
        void Init();
        void InitBasicSettings();
        void SetAtrributes();
        void SetShaders();

        void SetCamera(Core::CustomShader* shader, const size_t& e, const glm::mat4& mvp);
		void SetLighting(Core::CustomShader* shader);

        void RenderChunks();
		void RenderSkybox();
		void RenderWeapon();
		void RenderUI();

        GLuint VAO, VBO, EBO;
        GLuint Sky_VAO, Sky_VBO;
        GLuint CBO;

        float width, height;
		glm::mat4 cameraViewMatrix;
		glm::mat4 cameraProjectionMatrix;

        GameScene* scene;
        sf::RenderWindow* window;

        // Test
        Core::CustomShader* shaderChunk;
		Core::CustomShader* shaderSky;
        unsigned int cubemapTexture;
    };
}
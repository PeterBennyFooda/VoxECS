#include "Game.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <SFML/OpenGL.hpp>

namespace VoxelWasteland 
{
    Game::Game() : running(false)
    {
        sf::ContextSettings settings;

		// IMPORTANT: set the depth bits to 24 to get the depth buffer working
        settings.depthBits = 24;  
        settings.majorVersion = 3;
        settings.minorVersion = 2;

        window = new sf::RenderWindow{ { WINDOW_WIDTH, WINDOW_HEIGHT }, APP_NAME, sf::Style::Default, settings};
        window->setVerticalSyncEnabled(true);

		scene = new GameScene();
    }

    Game::~Game()
    {
        delete window;
    }

    /*
    *  @brief main game loop for the game
    */
    void Game::Run()
    {
        Init();

        window->setActive(true);
        while (running)
        {
            PollEvents();
            Update();
            Render();
        }
        window->setActive(false);
    }

    /*
    *  @brief initialize the game
    */
    void Game::Init()
    {
        window->setFramerateLimit(60);
        running = true;

        // create the player entity
        Entity player = scene->CreateEntity<CTransform, CCamera, CControllable, CCharacter, CPhysics, CRenderable>();
        CCharacter* playerInfo = scene->GetComponent<CCharacter>(player);
		CPhysics* playerPhysics = scene->GetComponent<CPhysics>(player);
        playerInfo->id = 0;
        playerInfo->name = "Player";
		playerInfo->health = 100;
		playerPhysics->mass = 20.0f;
		playerPhysics->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		playerPhysics->halfHeight = DEFAULT_PLAYER_HEIGHT / 2.0f;
		playerPhysics->halfWidth = DEFAULT_PLAYER_WIDTH / 2.0f;
		playerPhysics->halfDepth = DEFAULT_PLAYER_WIDTH / 2.0f;
		scene->SetPlayer(player);

		scene->Init(window);
        clock.restart();
    }

    /*
    *  @brief update the game
    */
    void Game::Update()
    {
        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();
        
		scene->Update(deltaTime);
    }

    /*
    *  @brief render the game
    */
    void Game::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // clear the buffers
		//window->clear(sf::Color(50, 106, 235)); 			  // clear the window with a color
		window->clear();

        scene->Render();

        window->display();
    }

    /*
    *  @brief poll SFML events for the game
    */
    void Game::PollEvents()
    {
        for (auto event = sf::Event{}; window->pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }
    }
}

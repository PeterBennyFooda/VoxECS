#pragma once

namespace VoxelWasteland 
{
	// ========== Common settings ==========
	#define APP_NAME "Voxel Wasteland"
	#define WINDOW_WIDTH 1920u
	#define WINDOW_HEIGHT 1080u

	// ========== Camera settings ==========
	#define CAMERA_LOOK_SPEED 0.1f
	#define CAMERA_FOV 45.0f
	#define CAMERA_NEAR 0.1f
	#define CAMERA_FAR 200.0f
	#define CAMERA_MAX_PITCH 1.0f
	#define CAMERA_MIN_PITCH -1.25f

	// ========== Settings for Map ==========
	#define MAP_SIZE 16
	#define MAP_RENDER_DISTANCE 16

	// ========== Setting for Chunks ==========
	#define CHUNK_SIZE 16
	#define CHUNK_HEIGHT 48
	#define CHUNK_NOISE_SCALE 1.5f
	#define CHUNK_MIN_HEIGHT 2
	#define CHUNK_MAX_HEIGHT CHUNK_HEIGHT/3
	#define CHUNK_SURFACE_HEIGHT 10

	#define CHUNK_NOISE_SCALE_3D 1.0f
	#define CHUNK_ACCEPTABLE_NOISE_3D -0.00f

	// ========== Settings for Blocks ==========
	typedef std::string BlockName;

	#define BLOCK_SIZE 0.5f
	#define DEFAULT_BLOCK_PREFIX "VoxelWasteland::"
	#define DEFAULT_BLOCK "VoxelWasteland::air"

	struct BlockData
	{
		int id = -1;
		BlockName name = DEFAULT_BLOCK;
		std::vector<std::string> textures;
		std::vector<GLuint> textureIDs;
		bool shouldCullFace = true;
	};

	// ========== Settings for ECS ==========
	//#define MAX_ENTITIES SIZE_MAX
	#define MAX_ENTITIES 1000
	#define MAX_COMPONENTS 32

	// ========== Settings for Player ==========
	#define DEFAULT_MOVE_SPEED 6.5f
	#define DEFAULT_JUMP_FORCE 20.0f
	#define DEFAULT_SPRINT_MOD 1.75f
	#define DEFAULT_PLAYER_HEIGHT 1.8f
	#define DEFAULT_PLAYER_WIDTH 0.5f
	#define CREATIVE_MODE true

	// ========== Settings for HUD ==========
	#define HUD_FONT_SIZE 24
	#define HUD_CROSSHAIR_SIZE 30.0f

	// ========== Settings for Physics ==========
	#define GRAVITY -4.9f 

	// ========== For general rendering ==========
	#define DEFAUL_TEXTURE_TYPE "texture_diffuse"

	// Struct memory layout is guaranteed to be sequential
	// so all variables are directly translated to float(byte) array
	// in this case, we got 8 floats for each vertex and the size is 8 * 4 = 32 bytes
	// which is: (x, y, z) + (a, b, c) + (u, v)
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		GLfloat Occlusion;
		GLuint TextureIndex;
	};
	struct Vertex2D
	{
		glm::vec3 Position;
		glm::vec2 TexCoords;
	};
	struct Texture
	{
		unsigned int id;
		std::string type;
	};

	// Light attenuation
	// Distance   Constant   Linear   Quadratic
	// 7          1.0        0.7      1.8
	// 13         1.0        0.35     0.44
	// 20         1.0        0.22     0.20
	// 32         1.0        0.14     0.07
	// 50         1.0        0.09     0.032
	// 65         1.0        0.07     0.017
	// 100        1.0        0.045    0.0075
	// 160        1.0        0.027    0.0028
	// 200        1.0        0.022    0.0019
	// 325        1.0        0.014    0.0007
	// 600        1.0        0.007    0.0002
	// 3250       1.0        0.0014   0.000007

	#define DEFAULT_LIGHT_CONSTANT 1.0f
	#define DEFAULT_PLAYER_LIGHT_LINEAR  0.09f 
	#define DEFAULT_PLAYER_LIGHT_QUADRATIC 0.032f

	// An array of attributes which represents vertices and texture coordinates
	static constexpr GLfloat cube_buffer_data[] =
	{
		// positions            // texture coords
		// Front face (z = 0.5)
		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // Bottom-left
		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom-right
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
		-0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // Top-left
		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // Bottom-left

		// Back face (z = -0.5) 
		-0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-left (flipped)
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  // Top-left (flipped)
		 0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-right (flipped)
		 0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-right (flipped)
		 0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-right (flipped)
		-0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   // Bottom-left (flipped)

		// Left face (x = -0.5)
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  // Top-left
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-right 
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-right 
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-right
		-0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  // Bottom-left
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   // Top-left

		// Right face (x = 0.5) 
		0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // Top-right (flipped)
		0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  // Bottom-right (flipped)
		0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-left (flipped)
		0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-left (flipped)
		0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  // Top-left (flipped)
		0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   // Top-right (flipped)

		// Bottom face (y = -0.5)
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // Bottom-right
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  // Top-right
		-0.5f, -0.5f,  0.5f,   0.0f, 1.0f,  // Top-left
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom-left

		// Top face (y = 0.5) 
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // Top-left
		-0.5f,  0.5f,  0.5f,   0.0f, 0.0f,  // Bottom-left
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  // Bottom-right
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  // Bottom-right
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  // Top-right
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f   // Top-left
	};
}
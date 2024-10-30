#pragma once
#include "stb_image.h"
#include <nlohmann/json.hpp>

namespace VoxelWasteland::Core
{
	using Json = nlohmann::json;

	class ResourceHelper
	{
	public:

		/*
		* @brief load a texture
		* @param path to the texture
		*/
		inline static unsigned int LoadTexture(const char* path)
		{
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            int width, height, nrChannels;

            // tell stb_image.h to flip loaded texture's on the y-axis
            // cuz the texture coordinates are flipped in OpenGL
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);

            if (data)
            {
				if (nrChannels == 4)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				}
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                throw std::runtime_error("Failed to load texture");
            }
            stbi_image_free(data);

            return texture;
		}

		/*
		* @brief load a texture array
		* @param paths: the paths to the textures
		* @param w: width of the texture
		* @param h: height of the texture
		* @return the texture id
		*/
		inline static unsigned int LoadTextureArray(const std::vector<std::string>& paths, const int& w = 16, const int& h = 16)
		{
			unsigned int textureArray;
			glGenTextures(1, &textureArray);
			glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, w, h, paths.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			int width, height, nrChannels;
			for (unsigned int i = 0; i < paths.size(); i++)
			{
				stbi_set_flip_vertically_on_load(true);
				unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
				}
				else
				{
					std::cout << "Texture Array failed to load at path: " << paths[i] << std::endl;
				}
				stbi_image_free(data);
			}
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			return textureArray;
		}

		/*
		* @brief load a cubemap
		* @param faces: the faces of the cubemap (right, left, top, bottom, front, back
		* @return the texture id
		*/
		inline static unsigned int LoadCubemap(const std::vector<std::string>& faces)
		{
			unsigned int textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

			int width, height, nrChannels;
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			return textureID;
		}

		/*
		* @brief load shaders
		* @param vertex_file_path path to the vertex shader
		* @param fragment_file_path path to the fragment shader
		* @return the program id
		*/
		inline static GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
		{
			// Create the shaders
			GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
			GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

			// Read the Vertex Shader code from the file
			std::string VertexShaderCode;
			std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
			if (VertexShaderStream.is_open()) {
				std::stringstream sstr;
				sstr << VertexShaderStream.rdbuf();
				VertexShaderCode = sstr.str();
				VertexShaderStream.close();
			}
			else {
				printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
				getchar();
				return 0;
			}

			// Read the Fragment Shader code from the file
			std::string FragmentShaderCode;
			std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
			if (FragmentShaderStream.is_open()) {
				std::stringstream sstr;
				sstr << FragmentShaderStream.rdbuf();
				FragmentShaderCode = sstr.str();
				FragmentShaderStream.close();
			}

			GLint Result = GL_FALSE;
			int InfoLogLength;

			// Compile Vertex Shader
			printf("Compiling shader : %s\n", vertex_file_path);
			char const* VertexSourcePointer = VertexShaderCode.c_str();
			glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
			glCompileShader(VertexShaderID);

			// Check Vertex Shader
			glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
				glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
				printf("%s\n", &VertexShaderErrorMessage[0]);
			}

			// Compile Fragment Shader
			printf("Compiling shader : %s\n", fragment_file_path);
			char const* FragmentSourcePointer = FragmentShaderCode.c_str();
			glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
			glCompileShader(FragmentShaderID);

			// Check Fragment Shader
			glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
				glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				printf("%s\n", &FragmentShaderErrorMessage[0]);
			}

			// Link the program
			printf("Linking program\n");
			GLuint ProgramID = glCreateProgram();
			glAttachShader(ProgramID, VertexShaderID);
			glAttachShader(ProgramID, FragmentShaderID);
			glLinkProgram(ProgramID);

			// Check the program
			glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
			glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
				glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
				printf("%s\n", &ProgramErrorMessage[0]);
			}

			glDetachShader(ProgramID, VertexShaderID);
			glDetachShader(ProgramID, FragmentShaderID);

			glDeleteShader(VertexShaderID);
			glDeleteShader(FragmentShaderID);

			return ProgramID;
		}

		/*
		* @brief load a json file
		* @param path: the path to the json file
		* @return the json object
		*/
		inline static Json LoadJson(const std::string& path)
		{
			Json result;

			std::ifstream file(path);
			if (!file.is_open())
			{
				throw std::runtime_error("File not found.");
			}

			file >> result;			
			return result;
		}

		/*
		* @brief get the block json data
		*/
		inline const static std::unordered_map<int, BlockData>& GetBlockData()
		{
			if (blockList.empty())
			{
				LoadBlockData();
			}

			return blockMap;
		}

		/*
		* @brief get the default textures
		* @param id: the block id
		* @return a vector of default texture paths
		*/
		inline static std::vector<std::string> GetDefaultTextures(const int id)
		{
			std::vector<std::string> result;

			if (blockList.empty())
			{
				LoadBlockData();
			}

			Json block = blockList[id];
			
			if (block["texture"].is_array())
			{
				for (auto& tex : block["texture"])
				{
					result.push_back(tex);
				}
			}
			else
			{
				result.push_back(block["texture"]);
			}

			return result;
		}

	private:
		
		inline static void LoadBlockData()
		{
			if (!blockList.empty())
				return;

			std::string prefix = "asset/json/block/";
			for (auto& name : blockNames)
			{
				std::string path = prefix + name + ".json";
				Json j = ResourceHelper::LoadJson(path);
				blockList.push_back(j);
			}

			int texID = 0;
			for (auto& block : blockList)
			{
				int id = block["id"];
				blockMap[id].id = block["id"];
				blockMap[id].name = block["name"];
				blockMap[id].textures = GetDefaultTextures(id);

				for (int i = 0; i < blockMap[id].textures.size(); ++i)
				{
					blockMap[id].textureIDs.push_back(texID);
					texID++;
				}

				// if texure number is less than 6, repeat the texture
				while (blockMap[id].textureIDs.size() < 6)
				{
					blockMap[id].textureIDs.push_back(blockMap[id].textureIDs.back());
				}

				if (block["type"] == "leaves")
				{
					blockMap[id].shouldCullFace = false;
				}
			}
		}

		inline static std::vector<Json> blockList;
		inline static std::unordered_map<int, BlockData> blockMap;

		inline static std::vector<std::string> blockNames =
		{
			"bedrock",
			"stone",
			"sandstone",
			"sand",
			"dirt",
			"grass_block",
			"dark_oak_log",
			"dark_oak_leaves",
			"water"
		};
	};
}
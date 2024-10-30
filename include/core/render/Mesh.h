#pragma once

namespace VoxelWasteland::Core
{
	class CustomShader;

	class Mesh
	{
	public:
		Mesh();
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
		~Mesh();

		void Draw(CustomShader* shader);
		void Rebuild(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	private:
		void Bind();

		// render data
		unsigned int VAO, VBO, EBO;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
	};
}
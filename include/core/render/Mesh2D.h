#pragma once

namespace VoxelWasteland::Core
{
	class CustomShader;

	class Mesh2D
	{
	public:
		Mesh2D();
		Mesh2D(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
		~Mesh2D();

		void Draw(CustomShader* shader);

	private:
		void Bind();

		// render data
		unsigned int VAO, VBO, EBO;
		std::vector<Vertex2D> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
	};
}
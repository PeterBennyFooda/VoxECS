#include "core/render/Mesh.h"
#include "core/render/CustomShader.h"

namespace VoxelWasteland::Core
{
	Mesh::Mesh() : VAO(0), VBO(0), EBO(0)
	{
	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
		vertices(vertices), indices(indices), textures(textures), VAO(0), VBO(0), EBO(0)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		Bind();
	}

	Mesh::~Mesh()
	{
	}

	/*
	* @brief draw the mesh
	* @param shader: the shader to use to draw the mesh
	*/
	void Mesh::Draw(CustomShader* shader)
	{
		if(vertices.size() <= 0 || indices.size() <=0)
		{
			return;
		}

		// texture rendering
		// bind appropriate textures
		unsigned int diffuseNr = 1;

		glDepthFunc(GL_LESS);

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

			shader->SetInt("textureArray", i);
			glBindTexture(GL_TEXTURE_2D_ARRAY, textures[i].id);
		}

		// draw mesh
		Bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	/*
	*  @brief rebuild the mesh with new vertices and indices
	*  @param vertices: new vertices
	*  @param indices: new indices
	*/
	void Mesh::Rebuild(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
	{
		if (vertices.size() <= 0 || indices.size() <= 0)
			return;

		this->vertices = vertices;
		this->indices = indices;
		Bind();
	}

	/*
	* @brief create the mesh
	*/
	void Mesh::Bind()
	{
		if (vertices.size() <= 0 || indices.size() <= 0)
		{
			return;
		}

		// Bind and set vertex buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// Bind and set element buffer for indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			&indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		// vertex AO
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Occlusion));

		// vertex texture index
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, TextureIndex)); // Note that we have to use glVertexAttribIPointer for integer types

		//glBindVertexArray(0);
	}
}
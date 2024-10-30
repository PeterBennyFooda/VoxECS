#include "core/render/Mesh2D.h"
#include "core/render/CustomShader.h"

namespace VoxelWasteland::Core
{
	Mesh2D::Mesh2D() : VAO(0), VBO(0), EBO(0)
	{
	}

	Mesh2D::Mesh2D(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
		vertices(vertices),
		indices(indices),
		textures(textures),
		VAO(0), VBO(0), EBO(0)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		Bind();
	}

	Mesh2D::~Mesh2D()
	{
	}

	/*
	* @brief draw the mesh
	* @param shader: the shader to use to draw the mesh
	*/
	void Mesh2D::Draw(CustomShader* shader)
	{
		if (vertices.size() <= 0 || indices.size() <= 0)
		{
			return;
		}

		// texture rendering
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// for transparent texture
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

			shader->SetInt("texture0", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// draw mesh
		Bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	/*
	* @brief create the mesh
	*/
	void Mesh2D::Bind()
	{
		if (vertices.size() <= 0 || indices.size() <= 0)
		{
			return;
		}

		// Bind and set vertex buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), &vertices[0], GL_STATIC_DRAW);

		// Bind and set element buffer for indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			&indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);

		// vertex texture coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, TexCoords));
	}
}



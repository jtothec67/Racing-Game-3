#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>
#include <string>

namespace Renderer
{
	struct Vertex
	{
		glm::vec3 m_position;
		glm::vec2 m_texcoords;
		glm::vec3 m_normal;
	};

	struct Face
	{
		Vertex a;
		Vertex b;
		Vertex c;
	};

	class Mesh
	{
	public:
		Mesh();
		Mesh(std::string _meshType); // User specifies mesh type, if not one we know we make a normal mesh
		~Mesh() { Unload(); }

		void add(Face& _face);
		GLuint id(); // Returns vao id + initialises mesh if dirty
		GLsizei vertex_count() const;

		GLuint vao() { return m_vaoid; }
		GLuint vbo() { return m_vboid; } // May need to pass using reference

		bool IsSkybox() { return m_skybox; }
		bool IsText() { return m_text; }

		// Function to unload the mesh from the GPU
		void Unload();

	private:
		GLuint m_vaoid = 0;
		GLuint m_vboid = 0;

		bool m_dirty = true;

		bool m_skybox = false;
		bool m_text = false;

		std::vector<Face> m_faces;
	};
}
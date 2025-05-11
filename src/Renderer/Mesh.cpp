#include "Mesh.h"

#include <iostream>
#include <exception>

namespace Renderer
{
	Mesh::Mesh()
	{

	}

	Mesh::Mesh(std::string _meshType)
	{
		if (_meshType == "skybox")
		{
			m_skybox = true;
		}
		else if (_meshType == "text")
		{
			m_text = true;
		}
	}

	void Mesh::add(Face& _face)
	{
		if (m_skybox || m_text)
		{
			std::cout << "Cannot add faces to a skybox or text mesh" << std::endl;
			throw std::exception();
		}

		m_faces.emplace_back(_face);
		m_dirty = true;
	}

	GLuint Mesh::id()
	{
		if (m_dirty)
		{
			if (!m_skybox && !m_text)
			{
				std::vector<GLfloat> data;

				data.reserve(m_faces.size() * 8 * 3);

				for (int i = 0; i < m_faces.size(); i++)
				{
					data.push_back(m_faces[i].a.m_position.x);
					data.push_back(m_faces[i].a.m_position.y);
					data.push_back(m_faces[i].a.m_position.z);
					data.push_back(m_faces[i].a.m_texcoords.x);
					data.push_back(m_faces[i].a.m_texcoords.y);
					data.push_back(m_faces[i].a.m_normal.x);
					data.push_back(m_faces[i].a.m_normal.y);
					data.push_back(m_faces[i].a.m_normal.z);

					data.push_back(m_faces[i].b.m_position.x);
					data.push_back(m_faces[i].b.m_position.y);
					data.push_back(m_faces[i].b.m_position.z);
					data.push_back(m_faces[i].b.m_texcoords.x);
					data.push_back(m_faces[i].b.m_texcoords.y);
					data.push_back(m_faces[i].b.m_normal.x);
					data.push_back(m_faces[i].b.m_normal.y);
					data.push_back(m_faces[i].b.m_normal.z);

					data.push_back(m_faces[i].c.m_position.x);
					data.push_back(m_faces[i].c.m_position.y);
					data.push_back(m_faces[i].c.m_position.z);
					data.push_back(m_faces[i].c.m_texcoords.x);
					data.push_back(m_faces[i].c.m_texcoords.y);
					data.push_back(m_faces[i].c.m_normal.x);
					data.push_back(m_faces[i].c.m_normal.y);
					data.push_back(m_faces[i].c.m_normal.z);
				}

				// Create a new VBO on the GPU and bind it
				glGenBuffers(1, &m_vboid);

				if (!m_vboid)
				{
					throw std::exception();
				}

				glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
				glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data.at(0)), &data.at(0), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glBindVertexArray(m_vaoid);
				glBindBuffer(GL_ARRAY_BUFFER, m_vboid);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
					8 * sizeof(data.at(0)), (void*)0);
				glEnableVertexAttribArray(0);

				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
					8 * sizeof(data.at(0)), (void*)(3 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);

				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
					8 * sizeof(data.at(0)), (void*)(5 * sizeof(GLfloat)));
				glEnableVertexAttribArray(2);

				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				m_dirty = false;
			}
			else if (m_skybox)
			{
				glGenVertexArrays(1, &m_vaoid);
				glBindVertexArray(m_vaoid);

				glGenBuffers(1, &m_vboid);
				glBindBuffer(GL_ARRAY_BUFFER, m_vboid);

				// Full-screen quad vertices
				float quadVertices[] = {
					-1.0f,  1.0f, 0.0f,
					-1.0f, -1.0f, 0.0f,
					1.0f,  1.0f, 0.0f,
					1.0f, -1.0f, 0.0f,
				};

				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glBindVertexArray(0);

				m_dirty = false;
			}
			else if (m_text)
			{
				glGenVertexArrays(1, &m_vaoid);
				glGenBuffers(1, &m_vboid);
				glBindVertexArray(m_vaoid);
				glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);

				m_dirty = false;
			}
		}

		return m_vaoid;
	}

	GLsizei Mesh::vertex_count() const
	{
		return (GLsizei)m_faces.size() * 3;
	}

	void Mesh::Unload()
	{
		if (m_vaoid)
		{
			glDeleteVertexArrays(1, &m_vaoid);
			m_vaoid = 0;
			m_dirty = true;
		}

		if (m_vboid)
		{
			glDeleteBuffers(1, &m_vboid);
			m_vboid = 0;
			m_dirty = true;
		}
	}
}
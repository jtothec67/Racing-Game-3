#include "Shader.h"

#include <glm/ext.hpp>

#include <iostream>
#include <exception>

namespace Renderer
{

	Shader::Shader(const std::string& _vertpath, const std::string& _fragpath)
	{
		m_vertpath = _vertpath;
		m_fragpath = _fragpath;

		std::ifstream vfile(_vertpath);

		if (!vfile.is_open())
		{
			std::cout << "Couldn't open vertex shader: " << _vertpath << std::endl;
			throw std::exception();
		}

		std::string vline;

		while (!vfile.eof())
		{
			std::getline(vfile, vline);
			vline += "\n";
			m_vertsrc += vline;
		}


		std::ifstream lfile(_fragpath);

		if (!lfile.is_open())
		{
			std::cout << "Couldn't open fragment shader: " << _fragpath << std::endl;
			throw std::exception();
		}

		std::string lline;

		while (!lfile.eof())
		{
			std::getline(lfile, lline);
			lline += "\n";
			m_fragsrc += lline;
		}
	}

	GLuint Shader::id()
	{
		if (m_dirty)
		{
			GLuint v_id = glCreateShader(GL_VERTEX_SHADER);
			const GLchar* GLvertsrc = m_vertsrc.c_str();
			glShaderSource(v_id, 1, &GLvertsrc, NULL);
			glCompileShader(v_id);
			GLint success = 0;
			glGetShaderiv(v_id, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				std::cout << "Vertex shader failed to compile: " << m_vertpath << std::endl;
				throw std::exception();
			}


			GLuint f_id = glCreateShader(GL_FRAGMENT_SHADER);
			const GLchar* GLfragsrc = m_fragsrc.c_str();
			glShaderSource(f_id, 1, &GLfragsrc, NULL);
			glCompileShader(f_id);
			glGetShaderiv(f_id, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				std::cout << "Fragment shader failed to compile: " << m_fragpath << std::endl;
				throw std::exception();
			}


			m_id = glCreateProgram();

			glAttachShader(m_id, v_id);
			glAttachShader(m_id, f_id);

			glLinkProgram(m_id);
			glGetProgramiv(m_id, GL_LINK_STATUS, &success);

			if (!success)
			{
				std::cout << "Shader id failed to be created: " << std::endl << m_vertpath << std::endl << m_fragpath << std::endl;
				throw std::exception();
			}

			glDetachShader(m_id, v_id);
			glDeleteShader(v_id);
			glDetachShader(m_id, f_id);
			glDeleteShader(f_id);

			m_dirty = false;
		}

		return m_id;
	}

	void Shader::uniform(const std::string& _name, bool value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());

		glUseProgram(id());
		glUniform1i(loc, value);
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, int value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());

		glUseProgram(id());
		glUniform1i(loc, value);
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, float value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());

		glUseProgram(id());
		glUniform1f(loc, value);
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, const glm::mat4& value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());

		glUseProgram(id());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, const glm::vec3& value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());

		glUseProgram(id());
		glUniform3fv(loc, 1, glm::value_ptr(value));
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, const glm::vec4& value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());

		glUseProgram(id());
		glUniform4fv(loc, 1, glm::value_ptr(value));
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, std::vector<int> value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());
		glUseProgram(id());
		glUniform1iv(loc, value.size(), value.data());
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, std::vector<float> value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());
		glUseProgram(id());
		glUniform1fv(loc, value.size(), value.data());
		glUseProgram(0);
	}

	void Shader::uniform(const std::string& _name, std::vector<glm::vec3> value)
	{
		// Find uniform locations
		GLint loc = glGetUniformLocation(id(), _name.c_str());
		glUseProgram(id());
		glUniform3fv(loc, value.size(), glm::value_ptr(value[0]));
		glUseProgram(0);
	}

	void Shader::draw(Model* _model, std::vector<Texture*>& _textures)
	{
		glUseProgram(id());

		if (!_model->usesMaterials())
		{
			if (!_textures.empty())
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, _textures[0]->id());
				glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
			}
			GLuint legacyVAO = _model->vao_id();
			glBindVertexArray(legacyVAO);
			glDrawArrays(GL_TRIANGLES, 0, _model->vertex_count());
		}
		else
		{
			const auto& groups = _model->GetMaterialGroups();
			for (size_t i = 0; i < groups.size(); ++i)
			{
				if (i < _textures.size())
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, _textures[i]->id());
					glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
				}
				glBindVertexArray(groups[i].vao);
				glDrawArrays(GL_TRIANGLES, 0, groups[i].faces.size() * 3);
			}
		}
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void Shader::draw(Model* _model, Texture* _tex)
	{
		glUseProgram(id());
		glBindVertexArray(_model->vao_id());
		glBindTexture(GL_TEXTURE_2D, _tex->id());
		glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
		glDrawArrays(GL_TRIANGLES, 0, _model->vertex_count());
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	void Shader::draw(Mesh* _mesh)
	{
		glUseProgram(id());
		glBindVertexArray(_mesh->id());
		glDrawArrays(GL_TRIANGLES, 0, _mesh->vertex_count());
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void Shader::draw(Mesh* _mesh, Texture* _tex)
	{
		glUseProgram(id());
		glBindVertexArray(_mesh->id());
		glBindTexture(GL_TEXTURE_2D, _tex->id());
		glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
		glDrawArrays(GL_TRIANGLES, 0, _mesh->vertex_count());
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	void Shader::draw(Mesh& _mesh, Texture& _tex)
	{
		glUseProgram(id());
		glBindVertexArray(_mesh.id());
		glBindTexture(GL_TEXTURE_2D, _tex.id());
		glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
		glDrawArrays(GL_TRIANGLES, 0, _mesh.vertex_count());
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	void Shader::draw(Mesh& _mesh, GLuint _texId)
	{
		glUseProgram(id());
		glBindVertexArray(_mesh.id());
		glBindTexture(GL_TEXTURE_2D, _texId);
		glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
		glDrawArrays(GL_TRIANGLES, 0, _mesh.vertex_count());
		glUseProgram(0);
	}

	void Shader::draw(Model& _model, Texture& _tex)
	{
		glUseProgram(id());
		glBindVertexArray(_model.vao_id());
		glBindTexture(GL_TEXTURE_2D, _tex.id());
		glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
		glDrawArrays(GL_TRIANGLES, 0, _model.vertex_count());
		glUseProgram(0);
	}

	void Shader::draw(Model& _model, GLuint _texId)
	{
		glUseProgram(id());
		glBindVertexArray(_model.vao_id());
		glBindTexture(GL_TEXTURE_2D, _texId);
		glUniform1i(glGetUniformLocation(id(), "u_Texture"), 0);
		glDrawArrays(GL_TRIANGLES, 0, _model.vertex_count());
		glUseProgram(0);
	}

	void Shader::draw(Model& _model, Texture& _tex, RenderTexture& _renderTex)
	{
		_renderTex.bind();

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glViewport(0, 0, _renderTex.getWidth(), _renderTex.getHeight());

		glUseProgram(id());
		glBindVertexArray(_model.vao_id());
		glBindTexture(GL_TEXTURE_2D, _tex.id());
		glDrawArrays(GL_TRIANGLES, 0, _model.vertex_count());
		glUseProgram(0);

		_renderTex.unbind();

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	}

	void Shader::draw(Mesh& _mesh, Texture& _tex, RenderTexture& _renderTex)
	{
		_renderTex.bind();

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glViewport(0, 0, _renderTex.getWidth(), _renderTex.getHeight());

		glUseProgram(id());
		glBindVertexArray(_mesh.id());
		glBindTexture(GL_TEXTURE_2D, _tex.id());
		glDrawArrays(GL_TRIANGLES, 0, _mesh.vertex_count());
		glUseProgram(0);

		_renderTex.unbind();

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	}

	void Shader::drawSkybox(Mesh& _skyboxMesh, Texture& _tex)
	{
		// Change depth function so depth test passes when values are equal to depth buffer's content
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		glUseProgram(id());
		glBindTexture(GL_TEXTURE_CUBE_MAP, _tex.id());
		GLuint textureLocation = glGetUniformLocation(id(), "uTexEnv");
		glUniform1i(textureLocation, 0);
		glBindVertexArray(_skyboxMesh.id());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);

		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
	}

	void Shader::drawSkybox(Mesh* _skyboxMesh, Texture* _tex)
	{
		// Change depth function so depth test passes when values are equal to depth buffer's content
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		glUseProgram(id());
		glBindTexture(GL_TEXTURE_CUBE_MAP, _tex->id());
		GLuint textureLocation = glGetUniformLocation(id(), "uTexEnv");
		glUniform1i(textureLocation, 0);
		glBindVertexArray(_skyboxMesh->id());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);

		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
	}

	void Shader::drawText(Mesh& _mesh, Font& _font, const std::string& _text, float _x, float _y, float _scale)
	{
		// Call id incase mesh hasn't been generated yet
		_mesh.id();

		glUseProgram(id());
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(_mesh.vao());

		float copyX = _x;
		float currentLineWidth = 0.0f;
		float widestLineWidth = 0.0f;

		// iterate through all characters
		std::string::const_iterator c;
		for (c = _text.begin(); c != _text.end(); c++)
		{
			Character* ch = _font.GetCharacter(c);

			if (*c == '\n')
			{
				_y -= ((ch->Size.y)) * 1.3 * _scale;
				_x = copyX;
				currentLineWidth = 0.0f;
			}
			else
			{
				float xpos = _x + ch->Bearing.x * _scale;
				float ypos = _y - (ch->Size.y - ch->Bearing.y) * _scale;

				float w = ch->Size.x * _scale;
				float h = ch->Size.y * _scale;

				currentLineWidth += w;
				if (currentLineWidth > widestLineWidth) widestLineWidth = currentLineWidth;

				// update VBO for each character
				float vertices[6][4] = {
					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos,     ypos,       0.0f, 1.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },

					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },
					{ xpos + w, ypos + h,   1.0f, 0.0f }
				};
				// render glyph texture over quad
				glBindTexture(GL_TEXTURE_2D, ch->TextureID);
				// update content of VBO memory
				glBindBuffer(GL_ARRAY_BUFFER, _mesh.vbo());
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// render quad
				glDrawArrays(GL_TRIANGLES, 0, 6);
				// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				_x += (ch->Advance >> 6) * _scale; // bitshift by 6 to get value in pixels (2^6 = 64)
			}
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	void Shader::drawOutline(Model* _model)
	{
		glUseProgram(id());
		glBindVertexArray(_model->vao_id());
		glDrawArrays(GL_LINE_LOOP, 0, _model->vertex_count());
		glBindVertexArray(0);
		glUseProgram(0);
	}
}
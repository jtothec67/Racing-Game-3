#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Model.h"
#include "RenderTexture.h"
#include "Font.h"

#include <GL/glew.h>

#include <string>

namespace Renderer
{
	class Shader
	{
	public:
		Shader(const std::string& _vertpath, const std::string& _fragpath);
		GLuint id();

		void uniform(const std::string& _name, bool _value);
		void uniform(const std::string& _name, int _value);
		void uniform(const std::string& _name, float _value);
		void uniform(const std::string& _name, const glm::mat4& _value);
		void uniform(const std::string& _name, const glm::vec3& _value);
		void uniform(const std::string& _name, const glm::vec4& _value);
		void uniform(const std::string& _name, std::vector<int> _value);
		void uniform(const std::string& _name, std::vector<float> _value);
		void uniform(const std::string& _name, std::vector<glm::vec3> _value);

		void draw(Model* _model, std::vector<Texture*>& _textures);
		void draw(Mesh* _mesh);
		void draw(Model* _model, Texture* _tex);
		void draw(Mesh* _mesh, Texture* _tex);
		void draw(Mesh& _mesh, Texture& _tex);
		void draw(Mesh& _mesh, GLuint _texId);
		void draw(Model& _model, Texture& _tex);
		void draw(Model& _model, GLuint _texId);
		void draw(Model& _model, Texture& _tex, RenderTexture& _renderTex);
		void draw(Mesh& _mesh, Texture& _tex, RenderTexture& _renderTexId);
		void drawSkybox(Mesh& _skyboxMesh, Texture& _tex);
		void drawSkybox(Mesh* _skyboxMesh, Texture* _tex);
		void drawText(Mesh& _mesh, Font& _font, const std::string& _text, float _x, float _y, float _scale);
		void drawOutline(Model* _model);

	private:
		GLuint m_id = 0;

		std::string m_vertpath;
		std::string m_fragpath;

		std::string m_vertsrc;
		std::string m_fragsrc;

		bool m_dirty = true;
	};
}
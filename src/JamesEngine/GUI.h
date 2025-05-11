#pragma once

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

#include <memory>

namespace JamesEngine
{

	class Texture;
	class Font;
	class Core;

	class GUI
	{
	public:
        GUI(std::shared_ptr<Core> _core);
		~GUI() {}

		int Button(glm::vec2 _position, glm::vec2 _size, std::shared_ptr<Texture> _texture);
		void Image(glm::vec2 _position, glm::vec2 _size, std::shared_ptr<Texture> _texture);
		void Text(glm::vec2 _position, float _size, glm::vec3 _colour, std::string _text, std::shared_ptr<Font> _font);
		void BlendImage(glm::vec2 _position, glm::vec2 _size, std::shared_ptr<Texture> _texture1, std::shared_ptr<Texture> _texture2, float _blendFactor);

	private:
		std::shared_ptr<Renderer::Shader> mShader = std::make_shared<Renderer::Shader>("../assets/shaders/GUIShader.vert", "../assets/shaders/GUIShader.frag");
		std::shared_ptr<Renderer::Mesh> mRect = std::make_shared<Renderer::Mesh>();

		std::shared_ptr<Renderer::Shader> mFontShader = std::make_shared<Renderer::Shader>("../assets/shaders/FontShader.vert", "../assets/shaders/FontShader.frag");
		std::shared_ptr<Renderer::Mesh> mTextRect = std::make_shared<Renderer::Mesh>("text");

		std::shared_ptr<Renderer::Shader> mBlendShader = std::make_shared<Renderer::Shader>("../assets/shaders/GUIShader.vert", "../assets/shaders/GUIBlendShader.frag");

		std::weak_ptr<Core> mCore;
	};

}
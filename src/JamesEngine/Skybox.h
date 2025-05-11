#pragma once

#include "SkyboxTexture.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

namespace JamesEngine
{

	class Core;

	class Skybox
	{
	public:
		Skybox(std::shared_ptr<Core> _core);

		void RenderSkybox();

		void SetTexture(std::shared_ptr<SkyboxTexture> _texture) { mTexture = _texture; }

	private:
		std::shared_ptr<SkyboxTexture> mTexture;
		std::shared_ptr<Renderer::Mesh> mMesh = std::make_shared<Renderer::Mesh>("skybox");
		std::shared_ptr<Renderer::Shader> mShader = std::make_shared<Renderer::Shader>("../assets/shaders/SkyboxShader.vert", "../assets/shaders/SkyboxShader.frag");

		std::weak_ptr<Core> mCore;
	};

}
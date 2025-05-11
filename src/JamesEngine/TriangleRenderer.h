#pragma once

#include "Component.h"

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace JamesEngine
{

	class TriangleRenderer : public Component
	{
	public:
		TriangleRenderer();
		void OnRender();

	private:
		std::shared_ptr<Renderer::Mesh> mMesh = std::make_shared<Renderer::Mesh>();
		std::shared_ptr<Renderer::Shader> mShader = std::make_shared<Renderer::Shader>("../assets/shaders/ObjShader.vert", "../assets/shaders/ObjShader.frag");
		std::shared_ptr<Renderer::Texture> mTexture = std::make_shared<Renderer::Texture>("../assets/images/cat.png");
	};

}
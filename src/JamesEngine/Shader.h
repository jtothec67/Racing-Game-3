#pragma once

#include "Resource.h"
#include "Renderer/Shader.h"

#include <memory>

namespace JamesEngine
{

	class ModelRenderer;

	class Shader : public Resource
	{
	public:
		void OnLoad() { mShader = std::make_shared<Renderer::Shader>(GetPath() + ".vert", GetPath() + ".frag"); }

	private:
		friend class ModelRenderer;

		std::shared_ptr<Renderer::Shader> mShader;
	};

}
#pragma once

#include "Resource.h"
#include "Renderer/Texture.h"

#include <memory>

namespace JamesEngine
{

	class ModelRenderer;
	class GUI;

	class Texture : public Resource
	{
	public:
		void OnLoad() { mTexture = std::make_shared<Renderer::Texture>(GetPath() + ".png"); }

	private:
		friend class ModelRenderer;
		friend class GUI;

		std::shared_ptr<Renderer::Texture> mTexture;
	};

}
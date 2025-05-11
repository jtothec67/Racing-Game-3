#pragma once

#include "Resource.h"
#include "Renderer/Texture.h"

#include <memory>

namespace JamesEngine
{

	class SkyboxTexture : public Resource
	{
	public:
		void OnLoad();

	private:
		friend class Skybox;

		std::shared_ptr<Renderer::Texture> mTexture;
	};

}
#pragma once

#include "Resource.h"
#include "Renderer/Font.h"

#include <memory>

namespace JamesEngine
{

	class ModelRenderer;

	class Font : public Resource
	{
	public:
		void OnLoad() { mFont = std::make_shared<Renderer::Font>(GetPath() + ".ttf"); }

	private:
		friend class ModelRenderer;
		friend class GUI;

		std::shared_ptr<Renderer::Font> mFont;
	};

}
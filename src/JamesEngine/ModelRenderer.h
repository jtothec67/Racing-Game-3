#pragma once

#include "Component.h"

#include <vector>

namespace JamesEngine
{
	class Model;
	class Texture;
	class Shader;

	class ModelRenderer : public Component
	{
	public:
		void OnInitialize();
		void OnRender();

		void SetModel(std::shared_ptr<Model> _model) { mModel = _model; }
		void AddTexture(std::shared_ptr<Texture> _texture) { mTextures.push_back(_texture); }
		void SetShader(std::shared_ptr<Shader> _shader) { mShader = _shader; }

		void SetSpecularStrength(float _strength) { mSpecularStrength = _strength; }

		void SetPositionOffset(glm::vec3 _offset) { mPositionOffset = _offset; }
		glm::vec3 GetPositionOffset() { return mPositionOffset; }

		void SetRotationOffset(glm::vec3 _offset) { mRotationOffset = _offset; }
		glm::vec3 GetRotationOffset() { return mRotationOffset; }

	private:
		std::shared_ptr<Model> mModel = nullptr;
		std::shared_ptr<Shader> mShader = nullptr;

		std::vector<std::shared_ptr<Texture>> mTextures;

		float mSpecularStrength = 1.f;

		glm::vec3 mPositionOffset{ 0 };
		glm::vec3 mRotationOffset{ 0 };
	};

}
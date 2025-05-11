#include "Skybox.h"

#include "Core.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace JamesEngine
{

	Skybox::Skybox(std::shared_ptr<Core> _core)
	{
		mCore = _core;
	}

	void Skybox::RenderSkybox()
	{
		if (!mTexture)
			return;

		glm::mat4 projection = mCore.lock()->GetCamera()->GetProjectionMatrix();
		glm::mat4 view = mCore.lock()->GetCamera()->GetViewMatrix();
		glm::mat4 invPV = glm::inverse(projection * view);

		mShader->uniform("u_InvPV", invPV);

		mShader->drawSkybox(mMesh.get(),mTexture->mTexture.get());
	}
}
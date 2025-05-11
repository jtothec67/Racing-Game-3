#include "TriangleRenderer.h"
#include "Entity.h"
#include "Transform.h"
#include "Core.h"
#include "Camera.h"

namespace JamesEngine
{

	TriangleRenderer::TriangleRenderer()
	{
		Renderer::Face face;
		face.a.m_position = glm::vec3(0.0f, 1.0f, 0.0f);
		face.b.m_position = glm::vec3(-1.0f, -1.0f, 0.0f);
		face.c.m_position = glm::vec3(1.0f, -1.0f, 0.0f);
		face.a.m_texcoords = glm::vec2(0.5f, -1.f);
		face.b.m_texcoords = glm::vec2(0.f, 0.f);
		face.c.m_texcoords = glm::vec2(1.f, 0.f);

		mMesh->add(face);
	}

	void TriangleRenderer::OnRender()
	{
		std::shared_ptr<Camera> camera = GetEntity()->GetCore()->GetCamera();

		mShader->uniform("u_Projection", camera->GetProjectionMatrix());

		mShader->uniform("u_View", camera->GetViewMatrix());

		Transform* transform = GetEntity()->GetComponent<Transform>().get();

		mShader->uniform("u_Model", transform->GetModel());

		mShader->uniform("u_Ambient", glm::vec3(1.f, 1.f, 1.f));

		mShader->draw(mMesh.get(), mTexture.get());
	}

}
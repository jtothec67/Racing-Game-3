#include "AudioSource.h"

#include "Transform.h"
#include "Entity.h"
#include "Core.h"
#include "Camera.h"

#ifdef _DEBUG

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#endif

namespace JamesEngine
{

	AudioSource::AudioSource()
	{
		alGenSources(1, &mSourceId);
		
		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

		SetPitch(1.f);
		SetGain(1.f);

		SetMinimumDistance(5.f);
		SetMaxDistance(30.f);
		SetRollOffFactor(1.f);
	}

	AudioSource::~AudioSource()
	{
		alDeleteSources(1, &mSourceId);
	}

	void AudioSource::OnTick()
	{
		std::shared_ptr<Core> core = GetEntity()->GetCore();
		glm::vec3 cameraPosition = core->GetCamera()->GetPosition();
		glm::vec3 cameraForward = core->GetCamera()->GetTransform()->GetForward();
		glm::vec3 cameraUp = -core->GetCamera()->GetTransform()->GetUp();

		alListener3f(AL_POSITION, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		float orientation[] = {
		cameraForward.x, cameraForward.y, cameraForward.z,
		cameraUp.x, cameraUp.y, cameraUp.z
		};
		alListenerfv(AL_ORIENTATION, orientation);


		alSource3f(mSourceId, AL_POSITION, GetPosition().x + mOffset.x, GetPosition().y + mOffset.y, GetPosition().z + mOffset.z);

		if (mLooping && !IsPlaying())
			Play();
	}

#ifdef _DEBUG
	void AudioSource::OnRender()
	{
		std::shared_ptr<Camera> camera = GetEntity()->GetCore()->GetCamera();

		mShader->uniform("projection", camera->GetProjectionMatrix());

		mShader->uniform("view", camera->GetViewMatrix());

		glm::mat4 mModelMatrix = glm::mat4(1.f);
		mModelMatrix = glm::translate(mModelMatrix, GetPosition() + mOffset);
		mModelMatrix = glm::scale(mModelMatrix, glm::vec3(mMaxDistance, mMaxDistance, mMaxDistance));

		mShader->uniform("model", mModelMatrix);

		mShader->uniform("outlineWidth", 1.f);

		mShader->uniform("outlineColor", glm::vec3(0, 0, 1));

		mShader->drawOutline(mModel.get());


		glm::mat4 mModelMatrix2 = glm::mat4(1.f);
		mModelMatrix2 = glm::translate(mModelMatrix2, GetPosition() + mOffset);
		mModelMatrix2 = glm::scale(mModelMatrix2, glm::vec3(mMinimumDistance, mMinimumDistance, mMinimumDistance));

		mShader->uniform("model", mModelMatrix2);


		mShader->drawOutline(mModel.get());
	}
#endif

	bool AudioSource::IsPlaying()
	{
		int state = 0; 
		alGetSourcei(mSourceId, AL_SOURCE_STATE, &state); 
		if (state == AL_PLAYING)
			return true;
		
		return false;
	}

	void AudioSource::Play()
	{
		if (mSound != nullptr)
			alSourcePlay(mSourceId);
	}

}
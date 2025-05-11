#pragma once

#include "Component.h"
#include "Sound.h"

#ifdef _DEBUG

#include "Renderer/Model.h"
#include "Renderer/Shader.h"

#endif

#include <AL/al.h>

namespace JamesEngine
{

	class AudioSource : public Component
	{
	public:
		AudioSource();
		~AudioSource();

		void OnTick();

#ifdef _DEBUG
		void OnRender();
#endif

		void SetSound(std::shared_ptr<Sound> _sound) { mSound = _sound; alSourcei(mSourceId, AL_BUFFER, mSound->mBufferId); }

		bool IsPlaying();
		void Play();

		void SetOffset(glm::vec3 _offset) { mOffset = _offset; }

		void SetPitch(float _pitch) { mPitch = _pitch; alSourcef(mSourceId, AL_PITCH, mPitch); }
		void SetGain(float _gain) { mGain = _gain; alSourcef(mSourceId, AL_GAIN, mGain); }
		void SetLooping(bool _looping) { mLooping = _looping; }

		void SetMinimumDistance(float _minimumDistance) { mMinimumDistance = _minimumDistance; alSourcef(mSourceId, AL_REFERENCE_DISTANCE, mMinimumDistance); }
		void SetMaxDistance(float _maxDistance) { mMaxDistance = _maxDistance; alSourcef(mSourceId, AL_MAX_DISTANCE, mMaxDistance); }
		void SetRollOffFactor(float _rollOffFactor) { mRollOffFactor = _rollOffFactor; alSourcef(mSourceId, AL_ROLLOFF_FACTOR, mRollOffFactor); }

	private:
		std::shared_ptr<Sound> mSound = nullptr;

		glm::vec3 mOffset = glm::vec3(0.f, 0.f, 0.f);

		ALenum mFormat = 0;
		ALsizei mFrequency = 0;

		ALuint mBufferId = 0;
		ALuint mSourceId = 0;

		bool mLooping = false;

		float mPitch;
		float mGain;

		float mMaxDistance;
		float mMinimumDistance;
		float mRollOffFactor;

#ifdef _DEBUG

		std::shared_ptr<Renderer::Model> mModel = std::make_shared<Renderer::Model>("../assets/shapes/sphere.obj");
		std::shared_ptr<Renderer::Shader> mShader = std::make_shared<Renderer::Shader>("../assets/shaders/OutlineShader.vert", "../assets/shaders/OutlineShader.frag");

#endif
	};

}
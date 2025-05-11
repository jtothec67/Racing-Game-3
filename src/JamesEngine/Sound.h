#pragma once

#include "Resource.h"

#include <AL/al.h>

namespace JamesEngine
{

	class AudioSource;

	class Sound : public Resource
	{
	public:
		void OnLoad();

	private:
		friend class AudioSource;

		ALuint mBufferId = 0;
		ALenum mFormat = 0;
		ALsizei mFrequency = 0;
	};

}
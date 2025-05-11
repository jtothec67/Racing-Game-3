#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace JamesEngine
{

	class Audio
	{
	public:
		Audio();
		~Audio();

	private:
		ALCcontext* mContext;
		ALCdevice* mDevice;
	};

}
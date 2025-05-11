#include "Audio.h"

#include <stdexcept>

namespace JamesEngine
{
	
	Audio::Audio()
	{
        mDevice = alcOpenDevice(0);

        if (!mDevice)
        {
            throw std::runtime_error("Failed to open audio device");
        }

        mContext = alcCreateContext(mDevice, 0);

        if (!mContext)
        {
            alcCloseDevice(mDevice);
            throw std::runtime_error("Failed to create audio context");
        }

        if (!alcMakeContextCurrent(mContext))
        {
            alcDestroyContext(mContext);
            alcCloseDevice(mDevice);
            throw std::runtime_error("Failed to make context current");
        }
	}

    Audio::~Audio()
    {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(mContext);
        alcCloseDevice(mDevice);
    }

}
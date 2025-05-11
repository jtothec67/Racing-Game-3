#include "Sound.h"

#include <vector>
#include <stdexcept>

#include "stb_vorbis.c"

namespace JamesEngine
{
	
	void Sound::OnLoad()
	{
		std::vector<unsigned char> data;

		int channels = 0;
		int sampleRate = 0;
		short* output = NULL;

		size_t samples = stb_vorbis_decode_filename((GetPath() + ".ogg").c_str(),
			&channels, &sampleRate, &output);

		if (samples == -1)
		{
			throw std::runtime_error("Failed to open file '" + (GetPath() + ".ogg") + "' for decoding");
		}

		// Record the format required by OpenAL
		if (channels < 2)
		{
			mFormat = AL_FORMAT_MONO16;
		}
		else
		{
			mFormat = AL_FORMAT_STEREO16;
		}

		// Copy (# samples) * (1 or 2 channels) * (16 bits == 2 bytes == short)
		data.resize(samples * channels * sizeof(short));
		memcpy(&data.at(0), output, data.size());

		// Record the sample rate required by OpenAL
		mFrequency = sampleRate;

		// Clean up the read data
		free(output);

		alGenBuffers(1, &mBufferId);

		alBufferData(mBufferId, mFormat, &data.at(0),
			static_cast<ALsizei>(data.size()), mFrequency);
	}

}
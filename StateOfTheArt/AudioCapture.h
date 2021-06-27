#pragma once
#include "pch.h"

class AudioCapture
{
	void AudioThread();
	bool requestStop;
	std::thread audioThread;
	std::vector<vec2> samples;
	std::mutex mutex;
public:
	std::vector<vec2> GetSamples()
	{
		const std::lock_guard<std::mutex> lock(mutex);
		return samples;
	}
	AudioCapture();
	~AudioCapture();
};
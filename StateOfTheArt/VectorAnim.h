#pragma once
#include "pch.h"

class VectorAnim
{
private:
	std::vector<std::vector<std::vector<vec2>>> frames;
	bool isLooping;
	bool isInterpolating;
public:
	static constexpr double frameDuration = 1.0 / 25.0;
	VectorAnim(const char* name, bool interp = true, bool loop = false);
	double duration()
	{
		return (double)frames.size() * frameDuration;
	}
	const std::vector<std::vector<vec2>> getShapeFromTime(double time);
};


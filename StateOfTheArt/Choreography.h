#pragma once

#include "VectorAnim.h"

class Entry
{
public:
	double time;
	int animIdex;
	int	startFrame;
	int endFrame;
};


class Choreography
{
private:
	std::vector<Entry> EntryVector;
	std::vector<VectorAnim> AnimVector;
public:
	Choreography(const char* splitTablePath, const char* sequencePath, const char* animDir);
	const std::vector<std::vector<vec2>> GetShapeFromTime(double time);
};

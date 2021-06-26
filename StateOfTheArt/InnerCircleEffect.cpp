#include "pch.h"
#include "effects.h"
#include "shapeutils.h"


std::vector<std::vector<Vertex>> InnerCircleEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float time)
{
	return std::move(Colorize(Resample(shapes, nbPoints), color0));
}

#include "pch.h"
#include "effects.h"
#include "shapeutils.h"


std::vector<std::vector<Vertex>> PlasmaEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float iTime)
{
	return std::move(Colorize(Resample(shapes, nbPoints), [iTime](vec2 p) { return 0.5f + 0.5f * cos(iTime + p.xyx / vec3(4096.0f) + vec3(0, 2, 4));; }));
}

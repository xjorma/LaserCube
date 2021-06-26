#include "pch.h"
#include "effects.h"
#include "shapeutils.h"

std::vector<std::vector<Vertex>> ColorEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float time)
{
	return std::move(Colorize(shapes, shapeColor));
}

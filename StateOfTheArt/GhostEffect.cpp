#include "pch.h"
#include "effects.h"
#include "shapeutils.h"


std::vector<std::vector<Vertex>> GhostEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float time)
{
	std::vector<std::vector<Vertex>> ret;
	trail.insert(trail.begin(), shapes);
	trail.resize(min((int)trail.size(), nbGhost));
	for (int i = 0; i < trail.size(); i++)
	{
		std::vector<std::vector<Vertex>> VGhost = Colorize(trail[i], mix(shapeColor, fadeColor, float(i) / float(trail.size() - 1)));
		ret.insert(ret.end(), VGhost.begin(), VGhost.end());
	}
	return std::move(ret);
}
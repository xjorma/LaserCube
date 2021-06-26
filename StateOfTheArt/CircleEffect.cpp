#include "pch.h"
#include "effects.h"
#include "boolean.h"
#include "shapeutils.h"

std::vector<vec2> BuildCircleShape(float radius, int subdiv)
{
	std::vector<vec2> ret;
	ret.reserve(subdiv + 1);
	for (int i = 0; i < subdiv + 1; i++)
	{
		float a = radians(360.f) * float(i) / float(subdiv);
		ret.emplace_back(cos(a) * radius + 2047.0f, sin(a) * radius + 2047.0f);
	}
	return std::move(ret);
}


std::vector<std::vector<Vertex>> CircleEffect::Apply(const std::vector<std::vector<vec2>> &shapes, float time)
{
	std::vector<vec2> circleShape = BuildCircleShape(1024.0f, 32);
	std::vector<std::vector<vec2>> circleShapes = { circleShape };
	if (shapes.size())
	{
		circleShapes = boolean(circleShape, shapes[0], true);
	}
	std::vector<std::vector<Vertex>> circleVShapes = Colorize(circleShapes, circleColor);
	std::vector<std::vector<Vertex>> vShapes = Colorize(shapes, shapeColor);
	std::vector<std::vector<Vertex>> vAllShape;
	vAllShape.reserve(circleVShapes.size() + vShapes.size());
	vAllShape.insert(vAllShape.end(), circleVShapes.begin(), circleVShapes.end());
	vAllShape.insert(vAllShape.end(), vShapes.begin(), vShapes.end());
	return std::move(vAllShape);
}
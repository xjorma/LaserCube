#include "pch.h"
#include "effects.h"
#include "boolean.h"
#include "shapeutils.h"
#include "AudioCapture.h"

std::vector<vec2> BuildLineShape(float height, int subdiv, AudioCapture* audioCapture)
{
	std::vector<vec2> ret;
	ret.reserve(subdiv);
	std::vector<vec2> sample = audioCapture->GetSamples();
	for (int i = 0; i < subdiv; i++)
	{
		vec2 v = vec2(mix(0.0f, 4095.0f, float(i) / float(subdiv + 1)), height);
		int idx = (int)floor((float(i) / float(subdiv + 1)) * float(sample.size() - 1));
		float s = dot(sample[idx], vec2(1));
		ret.push_back(v + vec2(0, s * 400.f));
	}
	return std::move(ret);
}


std::vector<std::vector<Vertex>> LineEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float time)
{
	std::vector<vec2> circleShape = BuildLineShape(2047.0f, 32, audioCapture);
	std::vector<std::vector<vec2>> circleShapes = { circleShape };
	if (shapes.size())
	{
		circleShapes = boolean(circleShape, shapes[0], false);
	}
	std::vector<std::vector<Vertex>> circleVShapes = Colorize(circleShapes, circleColor);
	std::vector<std::vector<Vertex>> vShapes = Colorize(shapes, shapeColor);
	std::vector<std::vector<Vertex>> vAllShape;
	vAllShape.reserve(circleVShapes.size() + vShapes.size());
	vAllShape.insert(vAllShape.end(), circleVShapes.begin(), circleVShapes.end());
	vAllShape.insert(vAllShape.end(), vShapes.begin(), vShapes.end());
	return std::move(vAllShape);
}
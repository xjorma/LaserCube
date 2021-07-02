#include "pch.h"
#include "effects.h"
#include "boolean.h"
#include "shapeutils.h"
#include "AudioCapture.h"

std::vector<vec2> BuildCircleShape(float radius, int subdiv, AudioCapture *audioCapture)
{
	std::vector<vec2> ret;
	ret.reserve(subdiv + 1);
	std::vector<vec2> sample = audioCapture->GetSamples();
	for (int i = 0; i < subdiv + 1; i++)
	{
		float a = radians(360.f) * float(i) / float(subdiv);
		vec2 v = vec2(cos(a), sin(a));
		int idx = (int)floor((v.x * 0.5f + 0.5f) * float(sample.size() - 1));
		float s = dot(sample[idx], vec2(1));
		float r = mix(radius, radius * 1.3f, s);
		ret.push_back(v * r + 2047.0f);
	}
	return ret;
}


std::vector<std::vector<Vertex>> CircleEffect::Apply(const std::vector<std::vector<vec2>> &shapes, float time)
{
	std::vector<vec2> circleShape = BuildCircleShape(radius, 64, audioCapture);
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
	return vAllShape;
}
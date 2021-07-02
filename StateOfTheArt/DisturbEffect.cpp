#include "pch.h"
#include "effects.h"
#include "shapeutils.h"
#include "AudioCapture.h"


inline std::vector<std::vector<vec2>> Pertubate(const std::vector<std::vector<vec2>>& shapes, std::function<vec2(vec2)> map)
{
	std::vector<std::vector<vec2>> ret;
	ret.reserve(shapes.size());
	for (const std::vector<vec2>& shape : shapes)
	{
		std::vector<vec2> vShape;
		vShape.reserve(shape.size());
		for (const vec2& p : shape)
		{
			vShape.push_back(p + map(p));
		}
		ret.push_back(std::move(vShape));
	}
	return std::move(ret);
}

std::vector<std::vector<Vertex>> DisturbEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float iTime)
{
	float strength = this->strength;
	std::vector<vec2> sample = audioCapture->GetSamples();
	return std::move(Colorize(Pertubate(Resample(shapes, nbPoints), [strength, sample](vec2 p) { return vec2(pow(sample[clamp(int((p.x * float(sample.size())/ 4096.0f)), 0, (int)sample.size())], vec2(2.f))) * strength;
}), shapeColor));
}

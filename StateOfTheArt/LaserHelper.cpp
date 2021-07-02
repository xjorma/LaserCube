#include "pch.h"
#include "effects.h"
#include "LaserHelper.h"


LaserSample ConvertToLaser(const Vertex &v)
{
	LaserSample sample;
	sample.x = (uint16_t)clamp(4095.0f - v.pos.x, 0.0f, 4095.0f);
	sample.y = (uint16_t)clamp(4095.0f - v.pos.y, 0.0f, 4095.0f);
	sample.r = (uint16_t)clamp(v.color.r * 256, 0.0f, 4095.0f);
	sample.g = (uint16_t)clamp(v.color.g * 256, 0.0f, 4095.0f);
	sample.b = (uint16_t)clamp(v.color.b * 128, 0.0f, 4095.0f);
	return sample;
}

std::vector<float> Measure(const std::vector<std::vector<Vertex>>& shapes, float& totalLen)
{
	totalLen = 0.0f;
	std::vector<float> measure;
	measure.reserve(shapes.size());
	for (const std::vector<Vertex>& shape : shapes)
	{
		float dist = 0.f;
		if (shape.size())
		{
			vec2 cur = shape[0].pos;
			for (int i = 1; i < shape.size(); i++)
			{
				dist += distance(cur, shape[i].pos);
				cur = shape[i].pos;
			}
		}
		measure.push_back(dist);
		totalLen += dist;
	}
	return std::move(measure);
}

void Resample(const std::vector<Vertex>& shape, std::vector<Vertex>& samples, int nbSamples, float measure)
{
	float inc = measure / (float)(nbSamples - 1);
	float cursor = 0.f;
	int curSpan = 0;
	for (int i = 0; i < nbSamples; i++)
	{
		vec2 v0;
		vec2 v1;
		vec3 c0;
		vec3 c1;
		float curLineDist;
		for (;;)
		{
			v0 = shape[curSpan].pos;
			c0 = shape[curSpan].color;
			v1 = shape[curSpan + 1].pos;
			c1 = shape[curSpan + 1].color;
			curLineDist = distance(v0, v1);
			if (cursor < curLineDist || curSpan >= shape.size() - 2)
				break;
			curSpan++;
			cursor -= curLineDist;
		}
		float alpha = cursor / curLineDist;
		samples.emplace_back(mix(v0, v1, alpha), mix(c0, c1, alpha));
		cursor += inc;
	}
}

inline void Resample(const std::vector<std::vector<Vertex>>& shapes, std::vector<Vertex>& samples, int nbSamples)
{
	float totalLen;
	int remainder = nbSamples;
	std::vector<float> measures = Measure(shapes, totalLen);
	if (shapes.size())
	{
		for (int i = 0; i < shapes.size() - 1; i++)
		{
			int nbAllocated = int(round(float(nbSamples) * measures[i] / totalLen));
			remainder -= nbAllocated;
			Resample(shapes[i], samples, nbAllocated, measures[i]);
		}
		Resample(shapes.back(), samples, remainder, measures.back());
	}
}

void ConvToLaserOS(const std::vector<std::vector<Vertex>>& shapes, std::vector<LaserdockSample> &samples, int nbSamples, float scale, float angle)
{

}

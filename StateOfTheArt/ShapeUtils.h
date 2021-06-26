#pragma once
#include "pch.h"

inline std::vector<float> Measure(const std::vector<std::vector<vec2>>& shapes, float& totalLen)
{
	totalLen = 0.0f;
	std::vector<float> measure;
	measure.reserve(shapes.size());
	for (const std::vector<vec2>& shape : shapes)
	{
		float dist = 0.f;
		if (shape.size())
		{
			vec2 cur = shape[0];
			for (int i = 1; i < shape.size(); i++)
			{
				dist += distance(cur, shape[i]);
				cur = shape[i];
			}
		}
		measure.push_back(dist);
		totalLen += dist;
	}
	return std::move(measure);
}

inline std::vector<vec2> Resample(const std::vector<vec2>& shape, int nbPoints, float measure)
{
	std::vector<vec2> ret;
	ret.reserve(nbPoints);
	float inc = measure / (float)(nbPoints - 1);
	float cursor = 0.f;
	int curSpan = 0;
	for (int i = 0; i < nbPoints; i++)
	{
		vec2 v0;
		vec2 v1;
		float curLineDist;
		for (;;)
		{
			v0 = shape[curSpan];
			v1 = shape[curSpan + 1];
			curLineDist = distance(v0, v1);
			if (cursor < curLineDist || curSpan >= shape.size() - 2)
				break;
			curSpan++;
			cursor -= curLineDist;
		}
		ret.push_back(mix(v0, v1, cursor / curLineDist));
		cursor += inc;
	}
	return std::move(ret);
}

inline std::vector<std::vector<vec2>> Resample(const std::vector<std::vector<vec2>>& shapes, int nbPoint)
{
	std::vector<std::vector<vec2>> ret;
	ret.reserve(shapes.size());
	float totalLen;
	int remainder = nbPoint;
	std::vector<float> measures = Measure(shapes, totalLen);
	if (shapes.size())
	{
		for (int i = 0; i < shapes.size() - 1; i++)
		{
			int nbAllocated = int(round(float(nbPoint) * measures[i] / totalLen));
			remainder -= nbAllocated;
			ret.push_back(Resample(shapes[i], nbPoint, measures[i]));
		}
		ret.push_back(Resample(shapes.back(), remainder, measures.back()));
	}
	return std::move(ret);
}


inline std::vector<std::vector<Vertex>> Colorize(const std::vector<std::vector<vec2>>& shapes, const vec3& col)
{
	std::vector<std::vector<Vertex>> ret;
	ret.reserve(shapes.size());
	for (const std::vector<vec2>& shape : shapes)
	{
		std::vector<Vertex> vShape;
		vShape.reserve(shape.size());
		for (const vec2& p : shape)
		{
			vShape.emplace_back(p, col);
		}
		ret.push_back(std::move(vShape));
	}
	return std::move(ret);
}

inline std::vector<std::vector<Vertex>> Colorize(const std::vector<std::vector<vec2>>& shapes, std::function<vec3(vec2)> map)
{
	std::vector<std::vector<Vertex>> ret;
	ret.reserve(shapes.size());
	for (const std::vector<vec2>& shape : shapes)
	{
		std::vector<Vertex> vShape;
		vShape.reserve(shape.size());
		for (const vec2& p : shape)
		{
			vShape.emplace_back(p, map(p));
		}
		ret.push_back(std::move(vShape));
	}
	return std::move(ret);
}
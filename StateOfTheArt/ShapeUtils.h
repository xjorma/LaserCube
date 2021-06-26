#pragma once
#include "pch.h"


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
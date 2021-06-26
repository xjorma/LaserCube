#include "pch.h"
#include "effects.h"
#include "boolean.h"
#include "shapeutils.h"


inline std::vector<std::vector<vec2>> Scale(const std::vector<std::vector<vec2>>& shapes, float scale)
{
	std::vector<std::vector<vec2>> ret;
	ret.reserve(shapes.size());
	for (const std::vector<vec2>& shape : shapes)
	{
		std::vector<vec2> vShape;
		vShape.reserve(shape.size());
		for (const vec2& p : shape)
		{
			vShape.push_back((p - 2047.f) * scale + 2047.f);
		}
		ret.push_back(std::move(vShape));
	}
	return std::move(ret);
}

std::vector<std::vector<Vertex>> MaskEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float time)
{
	std::vector<std::vector<vec2>> scaledShape = Scale(shapes, scale);
	std::vector<std::vector<Vertex>> ret = Colorize({ mask }, maskColor);
	for(const std::vector<vec2>& shape : scaledShape)
	{
		std::vector<std::vector<Vertex>> cutShape = Colorize({ boolean(shape, mask, true) }, shapeColor);
		ret.insert(ret.end(), cutShape.begin(), cutShape.end());
	}
	return std::move(ret);
}
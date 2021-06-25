#pragma once

#include "pch.h"

struct Vertex
{
	vec2 pos;
	vec3 color;
	Vertex(vec2 _pos, vec3 _color) : pos(_pos), color(_color)
	{
	}
};

class Effect
{
public:
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape) = 0;
};

class CircleEffect : public Effect
{
	float radius;
	vec3 shapeColor;
	vec3 circleColor; 
public:
	CircleEffect(float _radius, vec3 _shapeColor, vec3 _circleColor) : radius(_radius), shapeColor(_shapeColor), circleColor(_circleColor)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>> &shape) override;
};
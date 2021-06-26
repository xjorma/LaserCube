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

class ColorEffect : public Effect
{
	vec3 shapeColor;
public:
	ColorEffect(vec3 _shapeColor) : shapeColor(_shapeColor)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape) override;
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

class MaskEffect : public Effect
{
	vec3 shapeColor;
	vec3 maskColor;
	float scale;
	std::vector<vec2> mask;
public:
	MaskEffect(const std::vector<vec2> &_mask, vec3 _shapeColor, vec3 _maskColor, float _scale) : mask(_mask), shapeColor(_shapeColor), maskColor(_maskColor), scale(_scale)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape) override;
};


class Sequencer
{
public:
	struct SeqEntry
	{
		int	startTime;
		Effect* effect;
	};
private:
	std::vector<SeqEntry>	entries;
public:
	Sequencer(const std::vector<SeqEntry>& _entries) : entries(_entries)
	{
	}

	~Sequencer()
	{
		for (SeqEntry& entry : entries)
		{
			delete entry.effect;
		}
	}

	std::vector<std::vector<Vertex>> Tick(double time, const std::vector<std::vector<vec2>>& shape)
	{
		SeqEntry *curEffect = &entries[0];
		for (int i = 1; i < entries.size(); i++)
		{
			if (time >= double(entries[i].startTime) / 1000.0)
			{
				curEffect = &entries[i];
			}
		}
		return std::move(curEffect->effect->Apply(shape));
	}
};
#pragma once

#include "pch.h"

class AudioCapture;

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
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape, float time) = 0;
};

class ColorEffect : public Effect
{
	vec3 shapeColor;
public:
	ColorEffect(vec3 _shapeColor) : shapeColor(_shapeColor)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape, float time) override;
};


class CircleEffect : public Effect
{
	float radius;
	vec3 shapeColor;
	vec3 circleColor;
	AudioCapture *audioCapture;
public:
	CircleEffect(float _radius, vec3 _shapeColor, vec3 _circleColor, AudioCapture *_audioCapture) : radius(_radius), shapeColor(_shapeColor), circleColor(_circleColor), audioCapture(_audioCapture)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>> &shape, float time) override;
};

class LineEffect : public Effect
{
	int lines;
	vec3 shapeColor;
	vec3 circleColor;
	AudioCapture* audioCapture;
public:
	LineEffect(int _lines, vec3 _shapeColor, vec3 _circleColor, AudioCapture* _audioCapture) : lines(_lines), shapeColor(_shapeColor), circleColor(_circleColor), audioCapture(_audioCapture)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape, float time) override;
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
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape, float time) override;
};

class GhostEffect : public Effect
{
	vec3 shapeColor;
	vec3 fadeColor;
	int nbGhost;
	std::vector<std::vector<std::vector<vec2>>> trail;
public:
	GhostEffect(vec3 _shapeColor, vec3 _fadeColor, int _nbGhost) : shapeColor(_shapeColor), fadeColor(_fadeColor), nbGhost(_nbGhost)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape, float time) override;
};

class InnerCircleEffect : public Effect
{
	float period;
	float speed;
	int nbPoints;
	vec3 color0;
	vec3 color1;
public:
	InnerCircleEffect(vec3 _color0, vec3 _color1, float _period, int _nbPoints, float _speed) : color0(_color0), color1(_color1), period(_period), nbPoints(_nbPoints), speed(_speed)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape, float time) override;
};

class PlasmaEffect : public Effect
{
	int nbPoints;
public:
	PlasmaEffect(int _nbPoints) : nbPoints(_nbPoints)
	{
	}
	virtual std::vector<std::vector<Vertex>> Apply(const std::vector<std::vector<vec2>>& shape, float time) override;
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
		return std::move(curEffect->effect->Apply(shape, float(time)));
	}
};
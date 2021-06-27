#include "pch.h"
#include "effects.h"
#include "shapeutils.h"


std::vector<std::vector<Vertex>> InnerCircleEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float iTime)
{
	vec3 color0 = this->color0;
	vec3 color1 = this->color1;
	float period = this->period;
	float speed = this->speed;
	return std::move(Colorize(Resample(shapes, nbPoints), [color0, color1, period, iTime, speed](vec2 p) { return mix(color0, color1, cos(length(p - vec2(2047)) / period + iTime * speed) * 0.5f + 0.5f); }));
}

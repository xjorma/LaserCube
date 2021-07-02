#include "pch.h"
#include "effects.h"
#include "shapeutils.h"
#include "boolean.h"


inline vec3 applyFog(vec3  baseColor, vec3  fogColor, float distance, float strength)
{
	float fogAmount = 1.0f - exp(-distance * strength);
	return mix(baseColor, fogColor, fogAmount);
}

inline vec3 computeCenter(float d)
{
	return vec3(sin(d * 0.3 + 0.6), sin(d * 0.20), 0);
}


std::vector<std::vector<Vertex>> TunnelEffect::Apply(const std::vector<std::vector<vec2>>& dummy, float iTime)
{
	std::vector<std::vector<Vertex>> ret;
	bool tracing = false;
	const int nbPoints = 1000;
	float doffset = iTime * 5.f;
	vec3 bcenter = computeCenter(doffset);
	for (int i = 0; i < nbPoints; i++)
	{
		float d = float(i) * dist / float(nbPoints);
		vec3 center = computeCenter(d + doffset);
		float a = (d + doffset) * radians(360.0f) / (helicoidal);
		vec3 p = vec3(cos(a), sin(a), d) + center - bcenter;
		vec2 pp = vec2(p.xy) / (p.z + 0.5f);
		if (pp.x > -1.0f && pp.x < 1.0f && pp.y > -1.0f && pp.y < 1.0f)
		{
			if (!tracing)
			{
				tracing = true;
				std::vector<Vertex> v;
				ret.push_back(v);
			}
			vec3 col = applyFog(color, vec3(0.25, 0, 0), d, 0.1f);
			ret.back().emplace_back(pp * 2047.0f + 2047.0f, col);
		}
		else
		{
			tracing = false;
		}
	}

	return ret;
}

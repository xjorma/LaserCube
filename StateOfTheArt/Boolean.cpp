#include "pch.h"

#include "boolean.h"

bool GetLineIntersection(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float &t)
{
    vec2 s1 = p1 - p0;
    vec2 s2 = p3 - p2;

    float s;
    s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
    t = (s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        return true;
    }

    return false; // No collision
}

bool GetShapeIntersection(vec2 p0, vec2 p1, const std::vector<vec2>& mask, float& t)
{
    bool inter = false;
    t = std::numeric_limits<float>::max();
    for (int i = 0; i < mask.size() - 1; i++)
    {
        float curT;
        if (GetLineIntersection(p0, p1, mask[i], mask[i + 1], curT))
        {
            inter = true;
            t = min(curT, t);
        }
    }
    return inter;
}

bool isInside(const std::vector<vec2>& s, vec2 p)
{
    int nbInter = 0;
    for(int i = 0; i < s.size() - 1; i++)
    {
       float t;
        if (GetLineIntersection(s[i], s[i + 1], p, p + vec2(8192, 0), t))
            nbInter++;
    }
    return nbInter & 1;
}

std::vector<std::vector<vec2>> boolean(const std::vector<vec2>& s, const std::vector<vec2>& mask, bool traceInside)
{
    std::vector<std::vector<vec2>> ret;
    bool inside = isInside(mask, s[0]);
    bool tracing = (inside == traceInside);
    vec2 cur = s[0];
    float eps = 0.001f;
    if(tracing)
    {
        ret.push_back({ s[0] });
    }
    for(int id = 0; id < s.size() - 1;)
    {
        if(tracing)
        {
            float t;
            if (GetShapeIntersection(cur, s[id + 1], mask, t))
            {
                ret.back().push_back(cur + t * (s[id + 1] - cur));
                cur = cur + (t + eps) * (s[id + 1] - cur);
                tracing = false;
            }
            else
            {
                ret.back().push_back(s[id + 1]);
                cur = s[id + 1];
                id++;
            }
        }
        else
        {
            float t;
            if (GetShapeIntersection(cur, s[id + 1], mask, t))
            {
                ret.push_back({ cur + t * (s[id + 1] - cur) });
                cur = cur + (t + eps) * (s[id + 1] - cur);
                tracing = true;
            }
            else
            {
                cur = s[id + 1];
                id++;
            }
        }
    }
    return std::move(ret);
}
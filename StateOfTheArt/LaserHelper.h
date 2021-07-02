#pragma once
#include "LaserCube.h"


LaserSample ConvertToLaser(const Vertex& v, float intensity);

std::vector<float> Measure(const std::vector<std::vector<Vertex>>& shapes, float& totalLen);
void Resample(const std::vector<Vertex>& shape, std::vector<Vertex>& samples, int nbSamples, float measure);

vec2 ConvertToSamples(const std::vector<std::vector<Vertex>>& shapes, std::vector<LaserSample>& samples, vec2 lastPos, float drawStep, float moveStep, vec3 moveColor, float intensity);
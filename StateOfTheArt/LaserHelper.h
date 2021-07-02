#pragma once
#include "LaserCube.h"


LaserSample ConvertToLaser(const Vertex& v);

std::vector<float> Measure(const std::vector<std::vector<Vertex>>& shapes, float& totalLen);
void Resample(const std::vector<Vertex>& shape, std::vector<Vertex>& samples, int nbSamples, float measure);
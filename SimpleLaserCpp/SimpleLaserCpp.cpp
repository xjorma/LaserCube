#include "LaserCube.h"

int main()
{
    LaserCube laserCube;

    for(;;)
    {
        std::vector<LaserSample> samples;
        samples.reserve(256);
        float time = 0.0f;
        const float pi = 3.14159265359f;
        int pointInShape = 256;
        for (int i = 0; i < pointInShape; i++)
        {
            float p = float(i) / float(pointInShape);
            float intensity = 256.0f;
            LaserSample curSample;
            curSample.x = int(((sin(p * pi * 2))  * 0.7 / 2. + 0.5) * 0xfff);
            curSample.y = int(((cos(p * pi * 2))  * 0.7 / 2. + 0.5) * 0xfff);
            //curSample.r = int(pow((sin((p + (time * 1)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            //curSample.g = int(pow((sin((p + (time * 2)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            //curSample.b = int(pow((sin((p + (time * 3)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            curSample.r = intensity;
            curSample.g = intensity;
            curSample.b = 128;
            samples.push_back(curSample);
        }
        laserCube.DrawSamples(samples);
    }


}


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
        int pointInShape = 512;
        for (int i = 0; i < pointInShape; i++)
        {
            float p = float(i) / float(pointInShape);
            float intensity = 256.0f;
            LaserSample curSample;
            curSample.x = int(((sin(p * pi * 2.0f))  * 0.95 / 2. + 0.5) * 0xfff);
            curSample.y = int(((cos(p * pi * 2.0f))  * 0.95 / 2. + 0.5) * 0xfff);
            //curSample.r = int(pow((sin((p + (time * 1)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            //curSample.g = int(pow((sin((p + (time * 2)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            //curSample.b = int(pow((sin((p + (time * 3)) * (pi * 4)) / 2. + 0.5), 1) * intensity);
            curSample.r = 4095;
            curSample.g = 1024;
            curSample.b = 0;
            samples.push_back(curSample);
            //std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        laserCube.DrawSamples(samples,2000);

    }


}


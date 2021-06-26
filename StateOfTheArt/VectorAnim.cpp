#include "pch.h"
#include "VectorAnim.h"

vec2 ConvertToLaserOSRange(const vec2 &p)
{
    float x = (p.x / 255.0f) * 4095.0f;
    float y = ((p.y + (255.0f - 204.0f) * 0.5f) / 255.f) * 4095.0f;
    return vec2(x, y);
}

std::vector<vec2> MakeShape(const std::vector<int> &data, int idx, int length)
{
    std::vector<vec2> poly;
    poly.reserve(length + 1);
    for (int i = 0; i < length; ++i)
    {
        float x = float(data[idx + 1 + i * 2]);
        float y = float(data[idx + 0 + i * 2]);
        poly.push_back(ConvertToLaserOSRange(vec2(x,  y)));
    }
    poly.push_back(poly[0]);
    return std::move(poly);
}

std::vector<vec2> Tween(int tween_from, int tween_to, int tween_t, int tween_count, const std::vector<int>& data)
{
    std::vector<vec2> newShape;
    if ((data[tween_from] & 0xd0) != 0xd0)
    {
        printf("Unexpected tween idx (from) %X\n", data[tween_from]);
        return newShape;
    }
    if ((data[tween_to] & 0xd0) != 0xd0)
    {
        printf("Unexpected tween idx (from) %X\n", data[tween_to]);
        return newShape;
    }
    int tween_from_length = data[tween_from + 1];
    int tween_to_length = data[tween_to + 1];
    int tween_from_data = tween_from + 2;
    int tween_to_data = tween_to + 2;

    int points = max(tween_from_length, tween_to_length);
    newShape.reserve(points + 1);

    float point_multiplier_from = float(tween_from_length) / float(points);
    float point_multiplier_to = float(tween_to_length) / float(points);
    float amt = float(tween_t) / float(tween_count);

    for (int i = 0; i < points; ++i)
    {
        int point_from = int(floor(i * point_multiplier_from));
        int point_to = int(floor(i * point_multiplier_to));
        float alpha_from = fract(i * point_multiplier_from);
        float alpha_to = fract(i * point_multiplier_to);
        int idx_from0 = tween_from_data + (point_from * 2);
        int idx_to0 = tween_to_data + (point_to * 2);
        int idx_from1 = tween_from_data + (((point_from + 1) % tween_from_length) * 2);
        int idx_to1 = tween_to_data + (((point_to + 1) % tween_to_length) * 2);
        vec2 v_from = vec2(mix((float)data[idx_from0 + 1], (float)data[idx_from1 + 1], alpha_from), mix((float)data[idx_from0], (float)data[idx_from1], alpha_from));
        vec2 v_to = vec2(mix((float)data[idx_to0 + 1], (float)data[idx_to1 + 1], alpha_from), mix((float)data[idx_to0], (float)data[idx_to1], alpha_from));
        vec2 v = mix(v_from, v_to, amt);
        newShape.push_back(ConvertToLaserOSRange(v));
    }
    newShape.push_back(newShape[0]);
    return std::move(newShape);
}

std::vector<std::vector<vec2>> PlayFrame(const std::vector<int>& data, int idx)
{
    std::vector<std::vector<vec2>> shapes;
    int numShapes = data[idx++];
    shapes.reserve(numShapes);
    for (int i = 0; i < numShapes; ++i)
    {
        int cmd = data[idx++];
        if (cmd >= 0xd0 && cmd <= 0xdf)
        {
            int length = data[idx++];
            if (length)
            {
                shapes.push_back(std::move(MakeShape(data, idx, length)));
                idx += (length * 2);
            }
        }
        else if (cmd == 0xe6 || cmd == 0xe7 || cmd == 0xe8 || cmd == 0xf2)
        {
            int tween_from = idx - ((data[idx + 0] << 8) + (data[idx + 1]));
            int tween_to = idx + ((data[idx + 2] << 8) + (data[idx + 3]));
            int tween_t = data[idx + 4];
            int tween_count = data[idx + 5];
            idx = idx + 6;
            shapes.push_back(std::move(Tween(tween_from, tween_to, tween_t, tween_count, data)));
            if (shapes.back().size() == 0)  // handle some rare case where tween is invalid
            {
                shapes.pop_back();
            }
        }
        else
        {
            printf("unexpected draw cmd %X\n", cmd);
            break;
        }
        /*
        if(shapes.back().size())   // fix for "script0cbd82.json"
        {
            std::vector<vec2>& curShape = shapes.back();
            if (curShape.back().y != curShape[0].y)
            {
                curShape.emplace_back(curShape.back().x, curShape[0].y);
            }
        }
        */
    }
    return std::move(shapes);
}


VectorAnim::VectorAnim(const char *fileName, bool interp, bool loop) : isInterpolating(interp), isLooping(loop)
{
    std::ifstream myFile;
    myFile.open(fileName);
    if (myFile.is_open())
    {
        json js;
        myFile >> js;
        myFile.close();
        json indices = js["indices"];
        std::vector<int> data = js["data"];
        frames.reserve(indices.size());
        for (int idx : indices)
        {
            //idx = indices[0];
            frames.push_back(std::move(PlayFrame(data, idx)));
        }
    }
}

const std::vector<std::vector<vec2>> VectorAnim::getShapeFromTime(double time)
{
    int curFrameIdx = int(time / frameDuration);
    float alpha = (float)fract(time / frameDuration);
    if (isInterpolating && curFrameIdx != frames.size() - 1 && frames[curFrameIdx].size() == frames[curFrameIdx + 1].size())
    {
        std::vector<std::vector<vec2>> curFrame;
        curFrame.reserve(frames[curFrameIdx].size());
        for(int i = 0; i < (int)frames[curFrameIdx].size(); ++i)
        {
            if(frames[curFrameIdx][i].size() == frames[curFrameIdx + 1][i].size())
            {
                float maxDist = 0.0f;
                for (int j = 0; j < (int)frames[curFrameIdx][i].size(); j++)
                {
                    maxDist = max(maxDist, length(frames[curFrameIdx][i][j] - frames[curFrameIdx + 1][i][j]));
                }
                //printf("MaxDist: %f\n", maxDist);
                if (maxDist < 700.0f)
                {
                    std::vector<vec2> curShape;
                    curFrame.reserve(frames[curFrameIdx][i].size());

                    for (int j = 0; j < (int)frames[curFrameIdx][i].size(); j++)
                    {
                        curShape.push_back(mix(frames[curFrameIdx][i][j], frames[curFrameIdx + 1][i][j], alpha));
                    }
                    curFrame.push_back(std::move(curShape));
                }
                else
                {
                    curFrame.push_back(frames[curFrameIdx][i]);
                }
            }
            else
            {
                curFrame.push_back(frames[curFrameIdx][i]);
            }
        }
        return curFrame;
    }
    return frames[curFrameIdx];
}

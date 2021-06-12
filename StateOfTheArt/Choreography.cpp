#include "pch.h"
#include "Choreography.h"

template <class T> bool StreamFind(T &stream, char v)
{
    char c;
    while (stream.get(c))
    {
        if (c == v)
            return true;
    }
    return false;
}

template <class T> std::string ReadWord(T& stream, char v)
{
    std::string str;
    char c;
    while (stream.get(c))
    {
        if (c == v)
            break;
        str.push_back(c);
    }
    return std::move(str);
}


const std::vector<std::vector<vec2>> Choreography::GetShapeFromTime(double time)
{
    std::vector<std::vector<vec2>> shapes;
    for(const Entry& entry : EntryVector)
    {
        int delta = entry.endFrame - entry.startFrame;
        int deltaSign = sign(delta);
        int deltaAbs = abs(delta);
        double endTime = entry.time + (deltaAbs + 1) * VectorAnim::frameDuration;
        if (time > entry.time && time < endTime)
        {
            std::vector<std::vector<vec2>> AnimShapes = AnimVector[entry.animIdex].getShapeFromTime((time - entry.time) * deltaSign + entry.startFrame * VectorAnim::frameDuration);
            for (std::vector<vec2>& shape : AnimShapes)
            {
                for (vec2& v : shape)
                {
                    if (entry.flipHorizontal)
                        v.x = 4095.0f - v.x;
                    if (entry.flipVertical)
                        v.y = 4095.0f - v.y;
                }
                shapes.push_back(shape);
            }
        }
    }
    return std::move(shapes);
}


Choreography::Choreography(const char* splitTablePath, const char* sequencePath, const char* animDir)
{
    std::map<std::string, int> animMap;
    json splitTable;
    std::ifstream splitTableFile;
    splitTableFile.open(splitTablePath);
    if (splitTableFile.is_open())
    {
        splitTableFile >> splitTable;
        splitTableFile.close();
    }

    std::ifstream sequenceFile;
    sequenceFile.open(sequencePath);
    if (sequenceFile.is_open())
    {
        std::string line;
        bool flipVertical = false;
        bool flipHorizontal = false;
        while (std::getline(sequenceFile, line))
        {
            std::stringstream lineStream(line);
            int iTime;
            lineStream >> iTime;
            // 
            StreamFind(lineStream, '(');
            StreamFind(lineStream, '\'');
            std::string strRead;
            strRead = ReadWord(lineStream, '\'');
            if (strRead.compare(std::string("anim")))
            {
                if (!strRead.compare(std::string("scene")))
                {
                    flipVertical = false;
                    flipHorizontal = false;
                }
                else if (!strRead.compare(std::string("scene_options")))
                {
                    flipVertical = false;
                    flipHorizontal = false;
                    for (;;)
                    {
                        if (!StreamFind(lineStream, '\''))
                            break;
                        strRead = ReadWord(lineStream, '\'');
                        if (!strRead.compare(std::string("flip_vertical")))
                            flipVertical = true;
                        if (!strRead.compare(std::string("flip_horizontal")))
                            flipHorizontal = true;

                    }
                }
                continue;
            }
            StreamFind(lineStream, '{');
            StreamFind(lineStream, '\'');

            strRead = ReadWord(lineStream, '\'');
            if (strRead.compare(std::string("name")))
            {
                printf("\'name\' expected\n");
            }
            StreamFind(lineStream, '\'');
            std::string animNameStr = ReadWord(lineStream, '-');
            int animName = std::stoi(animNameStr, nullptr, 16);
            strRead = ReadWord(lineStream, '\'');
            int animIndex = std::stoi(strRead);
            StreamFind(lineStream, '\'');
            strRead = ReadWord(lineStream, '\'');
            if (strRead.compare(std::string("from")))
            {
                printf("\'from\' expected\n");
            }
            StreamFind(lineStream, ' ');
            strRead = ReadWord(lineStream, ',');
            int from = std::stoi(strRead);
            StreamFind(lineStream, '\'');
            strRead = ReadWord(lineStream, '\'');
            if (strRead.compare(std::string("to")))
            {
                printf("\'to\' expected\n");
            }
            StreamFind(lineStream, ' ');
            strRead = ReadWord(lineStream, ',');
            int to = std::stoi(strRead);
            // Fill Entry
            Entry entry;
            entry.time = (double)iTime / 1000.0;        // ms to seconds
            int offset = 0;                             // add proper offset base on splitmap table
            for (int i = 0; i < animIndex; i++)
                offset += splitTable[animNameStr.c_str()][i];
            entry.startFrame = from + offset;
            entry.endFrame = to + offset;
            entry.flipHorizontal = flipHorizontal;
            entry.flipVertical = flipVertical;
            // Load anim if needed
            if (animMap.count(animNameStr))
            {
                entry.animIdex = animMap[animNameStr];
            }
            else
            {
                std::string animFileName = std::string(animDir) + std::string("script") + animNameStr + std::string(".json");
                animMap[animNameStr] = (int)AnimVector.size();
                entry.animIdex = (int)AnimVector.size();
                AnimVector.push_back(VectorAnim(animFileName.c_str()));
            }
            EntryVector.push_back(entry);
        }
    }
}
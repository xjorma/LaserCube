#pragma once

#include "pch.h"


class RemapTime
{
private:
	struct RemapEntry
	{
		double dest;
		double org;
	};
	std::vector<RemapEntry> table;
public:
	RemapTime(const char *path)
	{
		std::ifstream remapFile;
		remapFile.open(path);
		if (remapFile.is_open())
		{
			std::string line;
			while (std::getline(remapFile, line))
			{
				RemapEntry entry;
				std::stringstream lineStream(line);
				int v;
				lineStream >> v;
				entry.dest = double(v - 1) / 60.0;
				lineStream >> v;
				entry.org = double(v - 1) / 60.0;
				table.push_back(entry);
			}
		}
	}
	double Convert(double realTime)
	{
		for(int i = 0; i < table.size(); i++)
		{
			if (table[i].org > realTime)
			{
				return mix(table[i - 1].dest, table[i].dest, (realTime - table[i - 1].org) / (table[i].org - table[i - 1].org));
			}
		}
		return table.back().dest + (realTime - table.back().org);
	}
};

#include "pch.h"
#include "effects.h"
#include "shapeutils.h"

// https://github.com/osresearch/vst/blob/master/teensyv/asteroids_font.c
#define P(x, y) vec2((x - 4.0f) / 10.0f, (y - 6.0f) / 10.0f)
#define FONT_UP vec2(-1, -1)
std::vector<std::vector<vec2>> font =
{
	{ P(4,0), P(3,2), P(5,2), P(4,0), FONT_UP, P(4,4), P(4,12)}, // !
	{ P(2,10), P(2,6), FONT_UP, P(6,10), P(6,6)}, // "
	{ P(0,4), P(8,4), P(6,2), P(6,10), P(8,8), P(0,8), P(2,10), P(2,2) }, // #
	{ P(6,2), P(2,6), P(6,10), FONT_UP, P(4,12), P(4,0)}, // $
	{ P(0,0), P(8,12), FONT_UP, P(2,10), P(2,8), FONT_UP, P(6,4), P(6,2) }, // %
	{ P(8,0), P(4,12), P(8,8), P(0,4), P(4,0), P(8,4) }, // &
	{ P(2,0), P(4,2) }, // '
	{ P(6,0), P(2,4), P(2,8), P(6,12)}, // (
	{ P(2,0), P(6,4), P(6,8), P(2,12)}, // )
	{ P(0,0), P(4,12), P(8,0), P(0,8), P(8,8), P(0,0)}, // *
	{ P(1,6), P(7,6), FONT_UP, P(4,9), P(4,3) }, // +
	{ P(2,0), P(4,2)}, // ,
	{ P(2,6), P(6,6)}, // -
	{ P(3,0), P(4,0)}, // .
	{ P(0,0), P(8,12)}, // /
	{ P(0,0), P(8,0), P(8,12), P(0,12), P(0,0), P(8,12) }, // 0
	{ P(4,0), P(4,12), P(3,10) }, // 1
	{ P(0,12), P(8,12), P(8,7), P(0,5), P(0,0), P(8,0) }, // 2
	{ P(0,12), P(8,12), P(8,0), P(0,0), FONT_UP, P(0,6), P(8,6) }, // 3
	{ P(0,12), P(0,6), P(8,6), FONT_UP, P(8,12), P(8,0) }, // 4
	{ P(0,0), P(8,0), P(8,6), P(0,7), P(0,12), P(8,12) }, // 5
	{ P(0,12), P(0,0), P(8,0), P(8,5), P(0,7) }, // 6
	{ P(0,12), P(8,12), P(8,6), P(4,0) }, // 7
	{ P(0,0), P(8,0), P(8,12), P(0,12), P(0,0), FONT_UP, P(0,6), P(8,6) }, //8
	{ P(8,0), P(8,12), P(0,12), P(0,7), P(8,5) }, // 9
	{ P(4,9), P(4,7), FONT_UP, P(4,5), P(4,3)}, // :
	{ P(4,9), P(4,7), FONT_UP, P(4,5), P(1,2)}, // ;
	{ P(6,0), P(2,6), P(6,12) }, // <
	{ P(1,4), P(7,4), FONT_UP, P(1,8), P(7,8)}, // =
	{ P(2,0), P(6,6), P(2,12) }, // >
	{ P(0,8), P(4,12), P(8,8), P(4,4), FONT_UP, P(4,1), P(4,0)}, // ?
	{ P(8,4), P(4,0), P(0,4), P(0,8), P(4,12), P(8,8), P(4,4), P(3,6) }, // @
	{ P(0,0), P(0,8), P(4,12), P(8,8), P(8,0), FONT_UP, P(0,4), P(8,4) }, // A
	{ P(0,0), P(0,12), P(4,12), P(8,10), P(4,6), P(8,2), P(4,0), P(0,0) },
	{ P(8,0), P(0,0), P(0,12), P(8,12)},
	{ P(0,0), P(0,12), P(4,12), P(8,8), P(8,4), P(4,0), P(0,0)},
	{ P(8,0), P(0,0), P(0,12), P(8,12), FONT_UP, P(0,6), P(6,6)},
	{ P(0,0), P(0,12), P(8,12), FONT_UP, P(0,6), P(6,6)},
	{ P(6,6), P(8,4), P(8,0), P(0,0), P(0,12), P(8,12)},
	{ P(0,0), P(0,12), FONT_UP, P(0,6), P(8,6), FONT_UP, P(8,12), P(8,0) },
	{ P(0,0), P(8,0), FONT_UP, P(4,0), P(4,12), FONT_UP, P(0,12), P(8,12) },
	{ P(0,4), P(4,0), P(8,0), P(8,12)},
	{ P(0,0), P(0,12), FONT_UP, P(8,12), P(0,6), P(6,0)},
	{ P(8,0), P(0,0), P(0,12)},
	{ P(0,0), P(0,12), P(4,8), P(8,12), P(8,0)},
	{ P(0,0), P(0,12), P(8,0), P(8,12)},
	{ P(0,0), P(0,12), P(8,12), P(8,0), P(0,0)},
	{ P(0,0), P(0,12), P(8,12), P(8,6), P(0,5)},
	{ P(0,0), P(0,12), P(8,12), P(8,4), P(0,0), FONT_UP, P(4,4), P(8,0) },
	{ P(0,0), P(0,12), P(8,12), P(8,6), P(0,5), FONT_UP, P(4,5), P(8,0) },
	{ P(0,2), P(2,0), P(8,0), P(8,5), P(0,7), P(0,12), P(6,12), P(8,10) },
	{ P(0,12), P(8,12), FONT_UP, P(4,12), P(4,0)},
	{ P(0,12), P(0,2), P(4,0), P(8,2), P(8,12)},
	{ P(0,12), P(4,0), P(8,12)},
	{ P(0,12), P(2,0), P(4,4), P(6,0), P(8,12)},
	{ P(0,0), P(8,12), FONT_UP, P(0,12), P(8,0)},
	{ P(0,12), P(4,6), P(8,12), FONT_UP, P(4,6), P(4,0)},
	{ P(0,12), P(8,12), P(0,0), P(8,0), FONT_UP, P(2,6), P(6,6)}, // Z
	{ P(6,0), P(2,0), P(2,12), P(6,12)}, // [
	{ P(2,6), P(6,10)}, // '\'
	{ P(2,0), P(6,0), P(6,12), P(2,12)},  // ]
	{ P(2,6), P(4,12), P(6,6)}, // ^
	{ P(0,0), P(8,0)}, // _
};

std::vector<std::vector<vec2>> DrawString(const std::string str, float scale)
{
	std::vector<std::vector<vec2>> ret;
	std::vector<vec2> curShape;

	float size = 4096.0f / float(str.size());

	for (int i = 0; i < str.size(); i++)
	{
		char c = str[i];
		const std::vector<vec2>& fontChar = font[c - 0x21];
		if (fontChar.size())
		{
			for (vec2 p : fontChar)
			{
				if (p.x == -1)
				{
					ret.push_back(curShape);
					curShape.clear();
				}
				else
				{
					curShape.push_back(((vec2(p.x, -p.y) * size + vec2(i * size, 0) + vec2(size * 0.5f, 2047)) - vec2(2047)) * scale + vec2(2047));
				}
			}
			ret.push_back(curShape);
			curShape.clear();
		}
	}

	return std::move(ret);
}

std::vector<std::vector<Vertex>> TextEffect::Apply(const std::vector<std::vector<vec2>>& shapes, float iTime)
{
	return std::move(Colorize(DrawString(str, scale), color));
}

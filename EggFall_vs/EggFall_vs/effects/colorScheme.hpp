#ifndef colorScheme_HPP
#define colorScheme_HPP

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

enum GRASS_COLOR_SCHEME
{
	GCS_NONE = 0,
	GCS_GREEN_A = 1,
	GCS_GREEN_B = 2,
	GCS_GREEN_C = 3,
	GCS_GREEN_D = 4,
	GCS_AUTUMN_A = 5,
	GCS_AUTUMN_B = 6,
	GCS_AUTUMN_C = 7,
	GCS_WINTER = 8,
	GCS_SAKURA = 9,
	GCS_RED_A = 10,
	GCS_RED_B = 11,
	GCS_PURPLE = 12,
	GCS_BLUE = 13,
};

class colorScheme
{
public:
	static Color* GetGrassColorScheme(GRASS_COLOR_SCHEME gcs);
	static void ColorizeTexture(Texture* texture, Color* sourceScheme, Color* targetScheme);
	static bool SchemeContainsColor(Color color, Color* scheme, int * id);
};
#endif

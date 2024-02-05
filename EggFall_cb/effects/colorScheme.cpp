#include "colorScheme.hpp"

using namespace std;
using namespace sf;

Color * colorScheme::GetGrassColorScheme(GRASS_COLOR_SCHEME gcs)
{
	switch (gcs)
	{
		case GCS_NONE:
			return new Color[3]{ Color(0, 0, 0), Color(0, 0, 0, 50), Color::Transparent };
			break;
		case GCS_GREEN_A:
			return new Color[3]{ Color(181, 230, 29), Color(107, 201, 28), Color(34, 177, 76) };
			break;
		case GCS_GREEN_B:
			return new Color[3]{ Color(97, 149, 0), Color(90, 138, 0), Color(83, 128, 0) };
			break;
		case GCS_GREEN_C:
			return new Color[3]{ Color(56, 92, 67), Color(52, 86, 62), Color(49, 80, 58) };
			break;
		case GCS_GREEN_D:
			return new Color[3]{ Color(0, 148, 44), Color(0, 138, 41), Color(0, 128, 38) };
			break;
		case GCS_AUTUMN_A:
			return new Color[3]{ Color(255, 231, 113), Color(224, 203, 100), Color(199, 178, 72) };
			break;
		case GCS_AUTUMN_B:
			return new Color[3]{ Color(248, 150, 26), Color(216, 131, 25), Color(192, 117, 23) };
			break;
		case GCS_AUTUMN_C:
			return new Color[3]{ Color(191, 110, 0), Color(181, 103, 0), Color(170, 100, 0) };
			break;
		case GCS_WINTER:
			return new Color[3]{ Color(212, 212, 212), Color(207, 207, 207), Color(201, 201, 201) };
			break;
		case GCS_SAKURA:
			return new Color[3]{ Color(255, 128, 160), Color(255, 117, 151), Color(255, 106, 143) };
			break;
		case GCS_RED_A:
			return new Color[3]{ Color(241, 76, 56), Color(222, 59, 39), Color(205, 49, 30) };
			break;
		case GCS_RED_B:
			return new Color[3]{ Color(153, 56, 49), Color(128, 39, 32), Color(112, 27, 21) };
			break;
		case GCS_PURPLE:
			return new Color[3]{ Color(187, 122, 255), Color(160, 101, 219), Color(134, 85, 185) };
			break;
		case GCS_BLUE:
			return new Color[3]{ Color(157, 173, 253), Color(143, 157, 229), Color(127, 139, 203) };
			break;
		default:
			break;
	}
}

void colorScheme::ColorizeTexture(Texture * texture, Color * sourceScheme, Color * targetScheme)
{
	try
	{
		Image img = texture->copyToImage();
		for (int x = 0; x < texture->getSize().x; x++)
		{
			for (int y = 0; y < texture->getSize().y; y++)
			{
				int id = 0;
				if (SchemeContainsColor(img.getPixel(x, y), sourceScheme, &id))
					img.setPixel(x, y, targetScheme[id]);
			}
		}
		texture->loadFromImage(img);
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
}

bool colorScheme::SchemeContainsColor(Color color, Color * scheme, int * id)
{
	try
	{
		int nbColors = sizeof(scheme) / sizeof(scheme[0]) + 1;
		for (int i = 0; i < nbColors; i++)
		{
			if (scheme[i] == color)
			{
				*id = i;
				return true;
			}
		}
		return false;
	}
	catch (exception)
	{
		return false;
	}
}

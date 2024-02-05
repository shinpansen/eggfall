#ifndef utils_HPP
#define utils_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <typeinfo>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

enum COLOR_SCHEME
{
	CS_WIZARD_RED,
	CS_WIZARD_PURPLE
};

class utils
{
public:
	//Variables
	static const float VIEW_WIDTH;
	static const float VIEW_HEIGHT;
	static const double PI;
	static pair<Color, Color>* wizardRedScheme;
	static pair<Color, Color>* wizardPurpleScheme;

	//Array constructor
	static pair<Color, Color>* colorScheme(COLOR_SCHEME scheme);

	//Methods
    static float StepCooldown(float cooldown, float step, float deltaTime, float valueToReach = 0.f);
	static float StepRotation(float rotationBase, float step);
	static int GetVectorRotation(Vector2f direction, int defaultAngle = 0);
	static float DistanceBetween(Vector2f pt1, Vector2f pt2);
	static float DegreeToRadian(float deg);
	static float RadianToDegree(float rad);
	static Vector2f RotateVector(Vector2f vect, float angleDeg);
	static int RandomNumber(int maxRand, int minRand);
	static bool StrContains(string searchStr, string findStr);
	static bool ListContainsText(vector<string> list, string text);
	static bool ListContainsInt(vector<int> list, int id);
	static Image ColorizeImage(string fileName, Color color);
	static Image ColorizeUsingScheme(string fileName, COLOR_SCHEME scheme);
	static void CopySprite(Sprite* targetSprite, Sprite* sourceSprite, Texture* texture, bool move = false);
	static void DrawLoopSprite(RenderWindow * window, Sprite * sprite);
	static vector<FloatRect> DrawLoopShape(RenderWindow * window, Shape * shape);
};
#endif

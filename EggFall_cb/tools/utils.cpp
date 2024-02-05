#include "utils.hpp"

using namespace std;
using namespace sf;

const float utils::VIEW_WIDTH = 1280.f; //800.f;
const float utils::VIEW_HEIGHT = 720.f;//600.f;
const double utils::PI = 3.14159265359;

pair<Color, Color>* utils::wizardRedScheme = colorScheme(CS_WIZARD_RED);
pair<Color, Color>* utils::wizardPurpleScheme = colorScheme(CS_WIZARD_PURPLE);

pair<Color, Color>* utils::colorScheme(COLOR_SCHEME scheme)
{
	return nullptr;

	pair<Color, Color>* schemeArray = new pair<Color, Color>[11];
	switch (scheme)
	{
		case CS_WIZARD_RED:
			schemeArray[0] = make_pair(Color(237, 28, 36), Color(255, 242, 0)); //Caster light color
			schemeArray[1] = make_pair(Color(136, 0, 21), Color(255, 201, 14)); //Caster dark color
			schemeArray[2] = make_pair(Color(0, 51, 72), Color(99, 0, 9)); //Cloth color 1 (darker)
			schemeArray[3] = make_pair(Color(0, 74, 106), Color(147, 0, 12)); //Cloth color 2
			schemeArray[4] = make_pair(Color(0, 83, 119), Color(165, 0, 14)); //Cloth color 3
			schemeArray[5] = make_pair(Color(0, 101, 145), Color(201, 0, 16)); //Cloth color 4 (lighter)
			schemeArray[6] = make_pair(Color(253, 150, 66), Color(253, 150, 66)); //Skin color 1
			schemeArray[7] = make_pair(Color(253, 171, 104), Color(253, 171, 104)); //Skin color 2
			schemeArray[8] = make_pair(Color(252, 224, 168), Color(252, 224, 168)); //Skin color 3
			schemeArray[9] = make_pair(Color(77, 77, 77), Color(77, 77, 77)); //Beard dark color
			schemeArray[10] = make_pair(Color(127, 127, 127), Color(127, 127, 127)); //Beard light color
			break;
		case CS_WIZARD_PURPLE:
			schemeArray[0] = make_pair(Color(237, 28, 36), Color(126, 237, 28));
			schemeArray[1] = make_pair(Color(136, 0, 21), Color(80, 136, 0));
			schemeArray[2] = make_pair(Color(0, 51, 72), Color(71, 0, 72));
			schemeArray[3] = make_pair(Color(0, 74, 106), Color(106, 0, 106));
			schemeArray[4] = make_pair(Color(0, 83, 119), Color(119, 0, 119));
			schemeArray[5] = make_pair(Color(0, 101, 145), Color(145, 0, 143));
			schemeArray[6] = make_pair(Color(253, 150, 66), Color(94, 57, 38));
			schemeArray[7] = make_pair(Color(253, 171, 104), Color(147, 92, 50));
			schemeArray[8] = make_pair(Color(252, 224, 168), Color(194, 129, 81));
			schemeArray[9] = make_pair(Color(77, 77, 77), Color(187, 187, 187));
			schemeArray[10] = make_pair(Color(127, 127, 127), Color(230, 230, 230));
			break;
	}
	return schemeArray;
}

float utils::StepCooldown(float cooldown, float step, float deltaTime, float valueToReach)
{
	float cooldownUpdate = cooldown;
	if (cooldown < valueToReach)
		cooldownUpdate = cooldown < valueToReach - step*deltaTime ? cooldown + step*deltaTime : valueToReach;
	else if (cooldown > valueToReach)
		cooldownUpdate = cooldown > valueToReach + step*deltaTime ? cooldown - step*deltaTime : valueToReach;
	return cooldownUpdate;
}

float utils::StepRotation(float rotationBase, float step)
{
	if (rotationBase < 0.f || rotationBase > 360.f)
		return 0.f;
	else if (rotationBase + step > 360.f)
		return (rotationBase + step) - 360.f;
	else if (rotationBase + step < 0.f)
		return 360.f + (rotationBase + step);
	else
		return rotationBase + step;
}

int utils::GetVectorRotation(Vector2f direction, int defaultAngle)
{
	if (direction.x == 0.f && direction.y == 0.f)
		return defaultAngle;

	//Calculating X Y value based on Sum 1
	Vector2f dir = direction;
	if (fabs(dir.x) + fabs(dir.y) != 1.f)
	{
		dir.x = dir.x / (fabs(dir.x) + fabs(dir.y));
		dir.x = roundf(dir.x * 100.f) / 100.f;
		float ySigne = dir.y / fabs(dir.y);
		dir.y = (1.f - fabs(dir.x))*ySigne;
	}

	//Calculating trigonometric quadrant (in deg)
	int initAngle = 0;
	if (dir.x >= 0.f)
		initAngle = dir.y <= 0.f ? 0 : 90;
	else
		initAngle = dir.y >= 0.f ? 180 : 270;

	//Return angle value in degree (0° to 360°)
	if (initAngle == 0 || initAngle == 180)
		return 90 * fabs(dir.x) + initAngle;
	else
		return 90 * (1.f - fabs(dir.x)) + initAngle;
}

float utils::DistanceBetween(Vector2f pt1, Vector2f pt2)
{
	return sqrt(pow(pt2.x - pt1.x, 2.f) + pow(pt2.y - pt1.y, 2.f));
}

float utils::DegreeToRadian(float deg)
{
	return deg * PI / 180.f;
}

float utils::RadianToDegree(float rad)
{
	return rad * 180.f / PI;
}

Vector2f utils::RotateVector(Vector2f vect, float angleDeg)
{
	float teta = DegreeToRadian(angleDeg);
	float rotMat[4] = { cos(teta), -sin(teta), sin(teta), cos(teta) };

	Vector2f vectRotated;
	vectRotated.x = vect.x * rotMat[0] + vect.y * rotMat[1];
	vectRotated.y = vect.x * rotMat[2] + vect.y * rotMat[3];
	return vectRotated;
}

int utils::RandomNumber(int maxRand, int minRand)
{
	if (maxRand < 0 || minRand < 0)
	{
		cout << "Max and min rand must be >= 0" << endl;
		return 0;
	}
	int random = minRand + (rand() % (maxRand - minRand + 1));
	return random;
}

bool utils::StrContains(string searchStr, string findStr)
{
	return searchStr.find(findStr) != std::string::npos;
}

bool utils::ListContainsText(vector<string> list, string text)
{
	for (int i = 0; i<list.size(); i++)
		if (list[i] == text)
			return true;
	return false;
}

bool utils::ListContainsInt(vector<int> list, int id)
{
	for (int i = 0; i<list.size(); i++)
		if (list[i] == id)
			return true;
	return false;
}

Image utils::ColorizeImage(string fileName, Color color)
{
	Image img;
	try
	{
		img.loadFromFile(fileName);
		for (int x = 0; x < img.getSize().x; x++)
			for (int y = 0; y < img.getSize().y; y++)
				if (img.getPixel(x, y).a != 0)
					img.setPixel(x, y, color);
		return img;
	}
	catch (exception)
	{
		return img;
	}
}

Image utils::ColorizeUsingScheme(string fileName, COLOR_SCHEME scheme)
{
	Image img;
	try
	{
		//Copying correct color scheme
		pair<Color, Color>* schemeArray;
		if (scheme == CS_WIZARD_RED)
			schemeArray = wizardRedScheme;
		else if (scheme == CS_WIZARD_PURPLE)
			schemeArray = wizardPurpleScheme;
		else
			return img;

		//Image coloration
		int nbColor = 11;// sizeof(schemeArray) / sizeof(schemeArray[0]);
		img.loadFromFile(fileName);
		for (int x = 0; x < img.getSize().x; x++)
			for (int y = 0; y < img.getSize().y; y++)
				for (int i = 0; i < nbColor; i++)
					if (img.getPixel(x, y) == schemeArray[i].first && schemeArray[i].first != schemeArray[i].second)
					{
						img.setPixel(x, y, schemeArray[i].second);
						break;
					}
		return img;
	}
	catch (exception)
	{
		return img;
	}
}

void utils::CopySprite(Sprite * targetSprite, Sprite * sourceSprite, Texture * texture, bool move)
{
	targetSprite->setTexture(*texture);
	targetSprite->setTextureRect(sourceSprite->getTextureRect());
	targetSprite->setScale(sourceSprite->getScale());
	targetSprite->setColor(sourceSprite->getColor());
	targetSprite->setOrigin(sourceSprite->getOrigin());
	targetSprite->setRotation(sourceSprite->getRotation());
	if (move)
		targetSprite->setPosition(sourceSprite->getPosition());
}

void utils::DrawLoopSprite(RenderWindow * window, Sprite * sprite)
{
	//Sprites clones creation
	Sprite clones[3];
	clones[0] = clones[1] = clones[2] = Sprite(*sprite);

	//Shift regarding source sprite location
	float xShift = sprite->getPosition().x < utils::VIEW_WIDTH / 2.f ? utils::VIEW_WIDTH : -utils::VIEW_WIDTH;
	float yShift = sprite->getPosition().y < utils::VIEW_HEIGHT / 2.f ? utils::VIEW_HEIGHT : -utils::VIEW_HEIGHT;

	//Positioning clones
	clones[0].setPosition(sprite->getPosition().x + xShift, sprite->getPosition().y);
	clones[1].setPosition(sprite->getPosition().x, sprite->getPosition().y + yShift);
	clones[2].setPosition(sprite->getPosition().x + xShift, sprite->getPosition().y + yShift);

	//Drawing sprite depending global bounds
	window->draw(*sprite);
	FloatRect viewBounds = FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
	for (int i = 0; i < 3; i++)
		if (clones[i].getGlobalBounds().intersects(viewBounds))
				window->draw(clones[i]);
}

vector<FloatRect> utils::DrawLoopShape(RenderWindow * window, Shape * shape)
{
	//Shapes initializaion regarding type
	Shape* clones[3];
	if ((string)(typeid(*shape).name()) == (string)(typeid(RectangleShape).name()))
		for (int i = 0; i < 3; i++)
			clones[i] = new RectangleShape(*dynamic_cast<RectangleShape*>(shape));
	else if ((string)(typeid(*shape).name()) == (string)(typeid(CircleShape).name()))
		for (int i = 0; i < 3; i++)
			clones[i] = new CircleShape(*dynamic_cast<CircleShape*>(shape));
	else
		for (int i = 0; i < 3; i++)
			clones[i] = new ConvexShape(*dynamic_cast<ConvexShape*>(shape));

	//Shift regarding source sprite location
	float xShift = shape->getPosition().x < utils::VIEW_WIDTH / 2.f ? utils::VIEW_WIDTH : -utils::VIEW_WIDTH;
	float yShift = shape->getPosition().y < utils::VIEW_HEIGHT / 2.f ? utils::VIEW_HEIGHT : -utils::VIEW_HEIGHT;

	//Positioning clones
	clones[0]->setPosition(shape->getPosition().x + xShift, shape->getPosition().y);
	clones[1]->setPosition(shape->getPosition().x, shape->getPosition().y + yShift);
	clones[2]->setPosition(shape->getPosition().x + xShift, shape->getPosition().y + yShift);

	//Drawing shapes depending global bounds
	vector<FloatRect> bounds;
	FloatRect viewBounds = FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
	bounds.push_back(shape->getGlobalBounds());
	window->draw(*shape);
	for (int i = 0; i < 3; i++)
	{
		if (clones[i]->getGlobalBounds().intersects(viewBounds))
		{
			bounds.push_back(clones[i]->getGlobalBounds());
			window->draw(*clones[i]);
		}
		delete clones[i]; //deallocate memory
	}
	return bounds;
}

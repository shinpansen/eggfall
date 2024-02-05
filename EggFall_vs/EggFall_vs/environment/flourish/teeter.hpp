#ifndef teeter_HPP
#define teeter_HPP

#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../../tools/files.hpp"
#include "../../tools/Box2DTools.hpp"
#include "../../effects/effects.hpp"
#include "../../effects/effectObj.hpp"

using namespace std;
using namespace sf;

class teeterNode
{
public:
	teeterNode();
	teeterNode(IntRect textureRect, Vector2f origin, Vector2f relativePos, bool invertRotation = true, bool displayInFront = true, 
		float speedReduction = 4.f, float angleRatio = 1.f, float angleReduction = 2.f);
	IntRect textureRect;
	Vector2f origin;
	Vector2f relativePos;
	bool invertRotation;
	bool displayInFront;
	float angleRatio;
	float speedReduction;
	float angleReduction;
	float angle;
	float angleToReach;
};

class teeter
{
public:
	~teeter();
	teeter();
	teeter(b2World * world, effects * effectBox, Texture * teeterTexture, Vector2f pos, int teeterHeight, Vector2f rectColorID, teeterNode root);
	void LoadNodes(vector<teeterNode> nodes);
    void PerformTeeter(float deltaTime);
    void CalcAngle();
    Color GetColor(int colorID);
    Vector2f GetPosition();
    void SetPosition(Vector2f pos);
    int GetColorID();
    void Step(float deltaTime);
	virtual void StepChild(float deltatime);
	void Display(RenderWindow* window);
	virtual void DisplayChild(RenderWindow* window);
protected:
    //Variables
    Vector2f _pos;
	Vector2f _rectColorID;
	Vector2i _contactPos;
	int _colorID;
	int _teeterHeight;

    //Objects
	b2World * _world;
    effects * _effectBox;
	teeterNode _root;
	vector<teeterNode> _nodes;

	//Graphics
	Texture * _teeterTexture;
    Sprite * _rootSprite;
	vector<Sprite*> _nodesSprites;
};
#endif

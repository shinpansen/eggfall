#ifndef spells_hpp
#define spells_hpp

#include <iostream>
#include <math.h>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/animation.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/files.hpp"
#include "../characters/hero.hpp"
#include "effects.hpp"
#include "sound.hpp"

using namespace std;
using namespace sf;

class hero;
class spells
{
public:
    spells();
    spells(b2World* world, effects* effectBox, sound* soundBox);
    void AddSpell(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                  int numAnimation, int angle, Vector2f speed, Color color);
    void DestroySpell(int i, bool showEffect, bool playSound);
    void DestroyAllSpells();
    void UpdateWizardsPointers(vector<hero*>* wizards);
    void PerformContacts(b2Body* spell, Vector2f* speedOfSpell, int spellNumber);
    bool isASpell(b2Body* spell, int* spellNumber);
    void RenderSpells(float deltaTime);
    void Display(RenderWindow* window);
private:
    float _deltaTime;
    files _files;
    b2World* _world;
    effects* _effectBox;
    sound* _soundBox;
    vector<Sprite*> _tiles;
    vector<animation*> _animations;
    vector<b2Body*> _spellsBodies;
    vector<CircleShape*> _lightOfSpells;
    vector<Vector2f> _speedOfSpell;
    vector<hero*>* _wizards;
    Texture _spellsTexture;
};
#endif

#include "spells.hpp"

static const float SCALE = 30.f;
static const double PI = 3.14159265359;
static const int SPELL_MAXSPEED = 30;
static const short GROUND_ENTITY = 0x0001;
static const short PLATFORM_ENTITY = 0x0002;
static const short BODY_ENTITY = 0x0008;

using namespace std;
using namespace sf;

spells::spells()
{
}

spells::spells(b2World* world, effects* effectBox, sound* soundBox)
{
    _world = world;
    _effectBox = effectBox;
    _soundBox = soundBox;
    if (!_spellsTexture.loadFromFile(_files.spellTile))
        cout << "Unable to find spells texture '" << _files.spellTile << "'" << endl;

    //First spell to run the machine
    AddSpell(Vector2i(-19,-19), 0.3, 1, 1, 6, 1, 0, Vector2f(0, 0), Color(0,0,0,0));
    _spellsBodies[0]->SetActive(false);
}

void spells::AddSpell(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                      int numAnimation, int angle, Vector2f speed, Color color)
{
    try
    {
        //Create spell objects
        _tiles.push_back(new Sprite());
        _animations.push_back(new animation());
        _speedOfSpell.push_back(speed);
        _lightOfSpells.push_back(new CircleShape());

        //Settings
        _tiles[_tiles.size()-1]->setTexture(_spellsTexture);
        _tiles[_tiles.size()-1]->setOrigin(width/2, height/2);
        _tiles[_tiles.size()-1]->setPosition(pos.x, pos.y);
        _tiles[_tiles.size()-1]->setRotation(angle);
        _tiles[_tiles.size()-1]->setScale(0.2,0.2);
        _spellsBodies.push_back(Box2DTools::CreateCircleBox(_world, pos, 20, 0.5, 1, 4));
        _spellsBodies[_spellsBodies.size()-1]->SetLinearVelocity(b2Vec2(speed.x, speed.y));
        _spellsBodies[_spellsBodies.size()-1]->ApplyForce(b2Vec2(0.f,-42*_spellsBodies[_spellsBodies.size()-1]->GetMass()), _spellsBodies[_spellsBodies.size()-1]->GetWorldCenter(), true);
        _spellsBodies[_spellsBodies.size()-1]->SetTransform(b2Vec2(pos.x/SCALE, pos.y/SCALE), angle*PI/180);
        *_animations[_animations.size()-1] =
            animation(_tiles[_tiles.size()-1], frameDelay, width, height, nbFrames, numAnimation+1, -1, true);
        _animations[_animations.size()-1]->SetAnimation(numAnimation, nbFrames);
        _lightOfSpells[_lightOfSpells.size()-1]->setRadius(18);
        _lightOfSpells[_lightOfSpells.size()-1]->setOrigin(18,18);
        _lightOfSpells[_lightOfSpells.size()-1]->setFillColor(Color(color.r, color.g, color.b, 100));
        _lightOfSpells[_lightOfSpells.size()-1]->setOutlineColor(Color(color.r, color.g, color.b, 50));
        _lightOfSpells[_lightOfSpells.size()-1]->setOutlineThickness(7);
        _lightOfSpells[_lightOfSpells.size()-1]->setPosition(pos.x, pos.y);
        _lightOfSpells[_lightOfSpells.size()-1]->setScale(0.2,0.2);
    }
    catch(exception e)
    {
        cout << "An error occurred while adding spell : " << e.what() << endl;
    }
}

void spells::DestroySpell(int i, bool showEffect, bool playSound)
{
    if(i > 0 && i < _spellsBodies.size())
    {
        float x = _spellsBodies[i]->GetPosition().x*SCALE;
        float y = _spellsBodies[i]->GetPosition().y*SCALE;
        int numAnimation = _animations[i]->GetAnimation();
        _world->DestroyBody(_spellsBodies[i]);
        _spellsBodies.erase(_spellsBodies.begin()+i);
        delete _animations[i];
        delete _tiles[i];
        delete _lightOfSpells[i];
        _animations.erase(_animations.begin()+i);
        _tiles.erase(_tiles.begin()+i);
        _speedOfSpell.erase(_speedOfSpell.begin()+i);
        _lightOfSpells.erase(_lightOfSpells.begin()+i);

        if(showEffect)
            _effectBox->AddEffect(Vector2i(x,y), 0.3, 75, 75, 6, numAnimation*2, false, 1);
        if(playSound)
            _soundBox->PlaySound(6, false);
    }
}

void spells::DestroyAllSpells()
{
    while(_spellsBodies.size() > 1)
        DestroySpell(1, false, false);
}

void spells::UpdateWizardsPointers(vector<hero*>* wizards)
{
    _wizards = wizards;
}

void spells::PerformContacts(b2Body* spell, Vector2f* speedOfSpell, int spellNumber)
{
    try
    {
        //Wizard vs spells
        for(int i=0 ; i<_wizards->size() ; i++)
        {
        for (b2Contact* contact = _world->GetContactList() ; contact ; contact = contact->GetNext())
        {
            if(!contact->IsTouching())
                continue;

            int spellNumberContact;
            //Ground contact
            if((contact->GetFixtureA()->GetBody() == spell ||
                contact->GetFixtureB()->GetBody() == spell) &&
               (contact->GetFixtureA()->GetFilterData().categoryBits == GROUND_ENTITY ||
                contact->GetFixtureB()->GetFilterData().categoryBits == GROUND_ENTITY ||
                contact->GetFixtureA()->GetFilterData().categoryBits == PLATFORM_ENTITY ||
                contact->GetFixtureB()->GetFilterData().categoryBits == PLATFORM_ENTITY))
            {
                DestroySpell(spellNumber, true, true);
                break;
            }
            //Contact with wizard
            else if((contact->GetFixtureA()->GetBody() == spell ||
                     contact->GetFixtureB()->GetBody() == spell) &&
                    (contact->GetFixtureA()->GetBody() == (*_wizards)[i]->GetBodyA() ||
                     contact->GetFixtureB()->GetBody() == (*_wizards)[i]->GetBodyA() ||
                     contact->GetFixtureA()->GetBody() == (*_wizards)[i]->GetBodyB() ||
                     contact->GetFixtureB()->GetBody() == (*_wizards)[i]->GetBodyB()))
            {
                //Get the right body (default or clone)
                b2Body* wizardBody;
                if(contact->GetFixtureA()->GetBody() == (*_wizards)[i]->GetBodyA() ||
                   contact->GetFixtureB()->GetBody() == (*_wizards)[i]->GetBodyA())
                    wizardBody = (*_wizards)[i]->GetBodyA();
                else
                    wizardBody = (*_wizards)[i]->GetBodyB();

                //{Counteracting
                if(((*_wizards)[i]->isCounteracting() == 1 &&
                     spell->GetPosition().x >= wizardBody->GetPosition().x) ||
                    ((*_wizards)[i]->isCounteracting() == -1 &&
                     spell->GetPosition().x < wizardBody->GetPosition().x) ||
                    ((*_wizards)[i]->isCounteracting() != 0 && fabs((*speedOfSpell).x) <= 0.5))
                {
                    if(fabs((*speedOfSpell).x) != fabs((*speedOfSpell).y)) //Straight
                    {
                        if(fabs((*speedOfSpell).x) > 0.5 && spell->GetPosition().x >= wizardBody->GetPosition().x)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x+25/SCALE, spell->GetPosition().y), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(2);
                            _effectBox->AddEffect(Vector2i(pos.x, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, true, 1);
                        }
                        else if(fabs((*speedOfSpell).x) > 0.5 && spell->GetPosition().x < wizardBody->GetPosition().x)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x-25/SCALE, spell->GetPosition().y), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(1);
                            _effectBox->AddEffect(Vector2i(pos.x, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, false, 1);
                        }
                        else if(fabs((*speedOfSpell).x) <= 0.5 && (*speedOfSpell).y < -0.5)//spell->GetPosition().y >= (*_wizards)[i]->GetBody()->GetPosition().y)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x, spell->GetPosition().y+65/SCALE), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(4);
                            Sprite* effect = _effectBox->AddEffect(Vector2i(spell->GetPosition().x*SCALE, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, false, 1);
                            effect->setRotation(270);
                        }
                        else if(fabs((*speedOfSpell).x) <= 0.5 && (*speedOfSpell).y > 0.5) //spell->GetPosition().y < (*_wizards)[i]->GetBody()->GetPosition().y)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x, spell->GetPosition().y-65/SCALE), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(3);
                            Sprite* effect = _effectBox->AddEffect(Vector2i(spell->GetPosition().x*SCALE, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, true, 1);
                            effect->setRotation(90);
                        }
                    }
                    else //Diagonal
                    {
                        if((*speedOfSpell).x > 0.5 && (*speedOfSpell).y > 0.5)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x-25/SCALE, spell->GetPosition().y-25/SCALE), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(5);
                            Sprite* effect = _effectBox->AddEffect(Vector2i(pos.x, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, false, 1);
                            effect->setRotation(45);
                        }
                        else if((*speedOfSpell).x > 0.5 && (*speedOfSpell).y < -0.5)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x-35/SCALE, spell->GetPosition().y+35/SCALE), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(7);
                            Sprite* effect = _effectBox->AddEffect(Vector2i(pos.x, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, false, 1);
                            effect->setRotation(315);
                        }
                        else if((*speedOfSpell).x < -0.5 && (*speedOfSpell).y > 0.5)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x+25/SCALE, spell->GetPosition().y-25/SCALE), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(6);
                            Sprite* effect = _effectBox->AddEffect(Vector2i(pos.x, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, false, 1);
                            effect->setRotation(-45);
                        }
                        else if((*speedOfSpell).x < -0.5 && (*speedOfSpell).y < -0.5)
                        {
                            spell->SetTransform(b2Vec2(spell->GetPosition().x+25/SCALE, spell->GetPosition().y+25/SCALE), spell->GetAngle());
                            Vector2f pos = (*_wizards)[i]->GetEffectAnglePosition(8);
                            Sprite* effect = _effectBox->AddEffect(Vector2i(pos.x, pos.y), 0.3, 75, 75, 6, _animations[spellNumber]->GetAnimation()*2+1, false, 1);
                            effect->setRotation(225);
                        }
                    }
                    _soundBox->PlaySound(9, false);
                    (*speedOfSpell).x = -(*speedOfSpell).x*1.2;
                    (*speedOfSpell).y = -(*speedOfSpell).y*1.2;
                    (*_wizards)[i]->hasCounteracted();

                    //Limiting spell max speed
                    if((*speedOfSpell).x > SPELL_MAXSPEED)
                        (*speedOfSpell).x = SPELL_MAXSPEED;
                    else if((*speedOfSpell).x < -SPELL_MAXSPEED)
                        (*speedOfSpell).x = -SPELL_MAXSPEED;
                    if((*speedOfSpell).y > SPELL_MAXSPEED)
                        (*speedOfSpell).y = SPELL_MAXSPEED;
                    else if((*speedOfSpell).y < -SPELL_MAXSPEED)
                        (*speedOfSpell).y = -SPELL_MAXSPEED;
                }
                //}
                //No countering
                else if((*_wizards)[i]->isCounteracting() == 0 ||
                  ((*_wizards)[i]->isCounteracting() == 1 &&
                   spell->GetPosition().x < wizardBody->GetPosition().x) ||
                  ((*_wizards)[i]->isCounteracting() == -1 &&
                   spell->GetPosition().x > wizardBody->GetPosition().x))
                {
                    //If the wizard has just fired his own spell
                    if((*_wizards)[i]->GetWizardType() == _animations[spellNumber]->GetAnimation() &&
                       (*_wizards)[i]->isSelfSpellInvincible())
                        return;

                    double x = (*speedOfSpell).x != 0 ? ((*speedOfSpell).x/fabs((*speedOfSpell).x))*15 : 0;
                    double y = (*speedOfSpell).y != 0 ? ((*speedOfSpell).y/fabs((*speedOfSpell).y))*15 : 0;
                    if(fabs(y) <= 0.1)
                        y = -2;
                    (*_wizards)[i]->SetStunned(Vector2f(x, y), 15, _animations[spellNumber]->GetAnimation());
                    DestroySpell(spellNumber, true, true);
                    break;
                }
            }
        }
        }
    }
    catch(exception e) { cout << e.what() << endl; }
}

bool spells::isASpell(b2Body* spell, int* spellNumber)
{
    for(int i=1 ; i<_spellsBodies.size() ; i++)
    {
        if(spell == _spellsBodies[i])
        {
            *spellNumber = i;
            return true;
        }
    }
    return false;
}

void spells::RenderSpells(float deltaTime)
{
    try
    {
        _deltaTime = deltaTime;
        //Spells encountering collision
        for(int i=1 ; i<_spellsBodies.size() ; i++)
            PerformContacts(_spellsBodies[i], &_speedOfSpell[i], i);

        //Spells vs spells
        for (b2Contact* contact = _world->GetContactList() ; contact ; contact = contact->GetNext())
        {
            int spellA;
            int spellB;
            if(contact->IsTouching() &&
               isASpell(contact->GetFixtureA()->GetBody(), &spellA) &&
               isASpell(contact->GetFixtureB()->GetBody(), &spellB))
            {
                DestroySpell(spellA, true, true);
                DestroySpell(spellB < spellA ? spellB : spellB-1, true, false);
            }
        }

        for(int i=1 ; i<_tiles.size() ; i++)
        {
            //Scale animation
            if(_tiles[i]->getScale().x < 1)
            {
                _tiles[i]->setScale(_tiles[i]->getScale().x+0.2, _tiles[i]->getScale().y+0.2);
                _lightOfSpells[i]->setScale(_tiles[i]->getScale());
            }

            //Variables
            float x = _spellsBodies[i]->GetPosition().x*SCALE;
            float y = _spellsBodies[i]->GetPosition().y*SCALE;
            float speed = fabs(_spellsBodies[i]->GetLinearVelocity().x)+fabs(_spellsBodies[i]->GetLinearVelocity().y);

            _effectBox->AddCircleLight(_lightOfSpells[i]->getFillColor(), Vector2f(x, y), 1);
            _spellsBodies[i]->SetLinearVelocity(b2Vec2(_speedOfSpell[i].x, _speedOfSpell[i].y));
            _spellsBodies[i]->ApplyForce(b2Vec2(0.f,-42*_spellsBodies[i]->GetMass()), _spellsBodies[i]->GetWorldCenter(), true); //Remove gravity
            _animations[i]->PerformAnimation(_deltaTime);
            _tiles[i]->setPosition(x, y);
            _lightOfSpells[i]->setPosition(x, y);

            //Speed scale
            if(_tiles[i]->getRotation() == 0 && _tiles[i]->getScale().x == 1)
            {
                _tiles[i]->setScale(1, 1-(speed/100));
                _lightOfSpells[i]->setScale(1, 1-(speed/100));
            }
            else if(_tiles[i]->getRotation() == 90 && _tiles[i]->getScale().y == 1)
            {
                _tiles[i]->setScale(1, 1-(speed/100));
                _lightOfSpells[i]->setScale(1-(speed/100), 1);
            }
        }
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void spells::Display(RenderWindow* window)
{
    //Drawing spells
    for(int i=1 ; i<_tiles.size() ; i++)
    {
        window->draw(*_lightOfSpells[i]);
        window->draw(*_tiles[i]);
    }
}

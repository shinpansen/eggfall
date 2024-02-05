#include "effects.hpp"

static const float SCALE = 30.f;
static const double PI = 3.14159265359;
using namespace std;
using namespace sf;

effects::effects()
{
    if (!_dustTexture.loadFromFile(_files.dustTile))
        cout << "Unable to find dust texture '" << _files.dustTile << "'" << endl;
    if (!_spellsTexture.loadFromFile(_files.spellExplosionTile))
        cout << "Unable to find spells texture '" << _files.spellExplosionTile << "'" << endl;
    if (!_wizardDyingTexture.loadFromFile(_files.wizardDying))
        cout << "Unable to find wizard dying texture '" << _files.wizardDying << "'" << endl;
}

Sprite* effects::AddEffect(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                           int numAnimation, bool reversed, int textureId)
{
    try
    {
        _tiles.push_back(new Sprite());
        _animations.push_back(new animation());
        if(textureId == 0)
            _tiles[_tiles.size()-1]->setTexture(_dustTexture);
        else if(textureId == 1)
            _tiles[_tiles.size()-1]->setTexture(_spellsTexture);
        else
            _tiles[_tiles.size()-1]->setTexture(_wizardDyingTexture);
        _tiles[_tiles.size()-1]->setOrigin(width/2, height/2);
        _tiles[_tiles.size()-1]->setPosition(pos.x, pos.y);
        *_animations[_animations.size()-1] = animation(_tiles[_tiles.size()-1], frameDelay, width, height, nbFrames, numAnimation+1, -1);
        _animations[_animations.size()-1]->SetAnimation(numAnimation, nbFrames);
        _animations[_animations.size()-1]->ReverseSprite(reversed);

        return _tiles[_tiles.size()-1];
    }
    catch(exception e)
    {
        cout << "An error occurred while adding effect : " << e.what() << endl;
    }
}

void effects::AddParticles(b2World* world, Color color, Vector2i pos, int minRand, int maxRand)
{
    try
    {
        srand(time(NULL));
        int random = rand()%(maxRand+minRand)+minRand;
        for(int i=0 ; i<random ; i++)
        {
            _particlesBodies.push_back(Box2DTools::CreateRectangleBox(world, pos, 10, 5, 0.5, 1, 0, 5));
            _particles.push_back(new RectangleShape());
            _particles[_particles.size()-1]->setSize(Vector2f(6,3));
            _particles[_particles.size()-1]->setFillColor(color);
            if(rand()%2 == 0)
                _particles[_particles.size()-1]->setScale(-1,1);
        }
    }
    catch(exception e)
    {
        cout << "An error occurred while adding particles : " << e.what() << endl;
    }
}

void effects::AddCircleLight(Color color, Vector2f pos, int number)
{
    try
    {
        for(int i=0 ; i<number ; i++)
        {
            int radius = 10;
            _stars.push_back(new CircleShape());
            _stars[_stars.size()-1]->setRadius(radius);
            _stars[_stars.size()-1]->setFillColor(Color(color.r, color.g, color.b, 50));
            _stars[_stars.size()-1]->setOrigin(radius, radius);
            _stars[_stars.size()-1]->setPosition(pos);
        }
    }
    catch(exception e)
    {
        cout << "An error occurred while adding star(s) : " << e.what() << endl;
    }
}

void effects::AddDust(Vector2f pos, bool toLeft, bool isStatic)
{
    try
    {
        //Random size and rotation speed
        int width = rand()%4+2;
        int rotationSpeed = rand()%15+5;
        int posX = rand()%6-3;
        int posY = rand()%6-3;
        float speedX = toLeft ? -fabs((float)posX*2.0) : fabs((float)posX*2.0);

        //Adding dust
        _dust.push_back(new RectangleShape());
        _dust[_dust.size()-1]->setFillColor(Color(235,235,200,230));
        _dust[_dust.size()-1]->setSize(Vector2f(width, width));
        _dust[_dust.size()-1]->setOrigin(Vector2f(width/2,width/2));
        _dust[_dust.size()-1]->setPosition(Vector2f(pos.x+posX, pos.y+posY));
        _dustRotation.push_back(toLeft ? rotationSpeed : -rotationSpeed);
        _dustSpeed.push_back(isStatic ? Vector2f(0,0) : Vector2f(speedX, -fabs((float)posY)));
    }
    catch(exception e)
    {
        cout << "An error occurred while adding dust : " << e.what() << endl;
    }
}

void effects::RenderEffects(b2World* world, RenderWindow* window, float deltaTime)
{
    try
    {
        _deltaTime = deltaTime;

        //Remove finished effects
        bool elementDeleted = false;
        for(int i=0 ; i<_animations.size() ; i++)
        {
            if(elementDeleted)
            {
                elementDeleted = false;
                i--;
            }

            if(_animations[i]->isLastFrame())
            {
                delete _animations[i];
                delete _tiles[i];
                _animations.erase(_animations.begin()+i);
                _tiles.erase(_tiles.begin()+i);
                elementDeleted = true;
            }
            else
                _animations[i]->PerformAnimation(_deltaTime);
        }

        //Remove static particles
        for(int i=0 ; i<_particlesBodies.size() ; i++)
        {
            if(_particlesBodies[i]->GetPosition().y*SCALE > 2000) //Outside map
            {
                world->DestroyBody(_particlesBodies[i]);
                delete _particles[i];
                _particles.erase(_particles.begin()+i);
                _particlesBodies.erase(_particlesBodies.begin()+i);
            }
            else if(fabs(_particlesBodies[i]->GetLinearVelocity().x) <= 0.0001 &&
               fabs(_particlesBodies[i]->GetLinearVelocity().y) <= 0.0001)
            {
               if(rand()%5 == 0)
               {
                   world->DestroyBody(_particlesBodies[i]);
                   delete _particles[i];
                   _particles.erase(_particles.begin()+i);
                   _particlesBodies.erase(_particlesBodies.begin()+i);
               }
            }
        }

        //Move and Remove stars
        for(int i=0 ; i<_stars.size() ; i++)
        {
            Color color = _stars[i]->getFillColor();
            _stars[i]->setFillColor(Color(color.r, color.g, color.b, color.a-2));
            _stars[i]->setScale(_stars[i]->getScale().x-0.04,_stars[i]->getScale().y-0.04);
            if(color.a-2 <= 0)
            {
                delete _stars[i];
                _stars.erase(_stars.begin()+i);
            }
        }

        //Moving and drawing dust
        for(int i=0 ; i<_dust.size() ; i++)
        {
            window->draw(*_dust[i]);
            Color color = _dust[i]->getFillColor();
            _dust[i]->setRotation(_dust[i]->getRotation()+_dustRotation[i]);
            _dust[i]->move(_dustSpeed[i]);
            _dustSpeed[i] = Vector2f(_dustSpeed[i].x*0.5, _dustSpeed[i].y*0.5);
            if(color.a > 50)
                _dust[i]->setFillColor(Color(color.r, color.g, color.b, color.a-5));
            else
            {
                delete _dust[i];
                _dust.erase(_dust.begin()+i);
                _dustRotation.erase(_dustRotation.begin()+i);
                _dustSpeed.erase(_dustSpeed.begin()+i);
            }
        }

        //Drawing effects
        for(int i=0 ; i<_tiles.size() ; i++)
            window->draw(*_tiles[i]);

        //Rendering particles
        for(int i=0 ; i<_particles.size() ; i++)
        {
            float x = _particlesBodies[i]->GetPosition().x*SCALE;
            float y = _particlesBodies[i]->GetPosition().y*SCALE;
            _particles[i]->setPosition(x, y);
            _particles[i]->setRotation(_particlesBodies[i]->GetAngle()*180/PI);
            window->draw(*_particles[i]);
        }

        //Drawing stars
        for(int i=0 ; i<_stars.size() ; i++)
            window->draw((*_stars[i]));
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void effects::Display(RenderWindow* window)
{
    //Drawing effects
    for(int i=0 ; i<_tiles.size() ; i++)
        window->draw(*_tiles[i]);

    //Drawing particles
    for(int i=0 ; i<_particles.size() ; i++)
        window->draw(*_particles[i]);

    //Drawing stars
    for(int i=0 ; i<_stars.size() ; i++)
        window->draw((*_stars[i]));
}

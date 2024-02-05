#include "animation.hpp"

using namespace std;
using namespace sf;

animation::animation()
{
}

animation::~animation()
{
}

animation::animation(Sprite* tile, float frameDelay, int width, int height, int nbFrames, int nbAnimations, 
					 int staticAnimation, bool disableScale, vector<Texture*> textures)
{
    //Variables
    _tile = tile;
    _width = width;
    _height = height;
    _nbFrames = nbFrames;
    _nbAnimations = nbAnimations;
	_nbAnimationsPerTile = nbAnimations;
    _animation = -1;
    _loop = 0;
    _frame = 0;
    _deltaTime = 1;
    _frameDelay = frameDelay;
    _reversed = false;
    _staticFrame = 0;
	_staticAnimation = -1;// staticAnimation;
    _disableScale = disableScale;
	_textures = textures;

    //Animation of falling - crush sprite
    _scaleFall = new float[10];
    _frameScaleFall = 9.f;
	_scaleFall[9] = 1.f;

    //Animation of jumping
    _scaleJump = new float[10];
    _frameScaleJump = 9.f;
	_scaleJump[9] = 1.f;

    //IntRect informations
    _borderTopLeft = Vector2i(0,0);
    _borderBottomRight = Vector2i(width, height);

	//If multiple textures
	if (textures.size() > 0)
		_nbAnimationsPerTile = textures[0]->getSize().y / height;

	//Start animation
    PerformAnimation(_deltaTime);
}

void animation::Reset(Sprite * tile, float frameDelay, int width, int height, int nbFrames, int nbAnimations, 
	                  int staticAnimation, bool disableScale, vector<Texture*> textures)
{
	//Variables
	_tile = tile;
	_width = width;
	_height = height;
	_nbFrames = nbFrames;
	_nbAnimations = nbAnimations;
	_nbAnimationsPerTile = nbAnimations;
	_animation = -1;
	_loop = 0;
	_frame = 0;
	_deltaTime = 1;
	_frameDelay = frameDelay;
	_reversed = false;
	_staticFrame = 0;
	_staticAnimation = -1;// staticAnimation;
	_disableScale = disableScale;
	_textures = textures;

	//Animation of falling - crush sprite
	_scaleFall = new float[10];
	_frameScaleFall = 9.f;
	_scaleFall[9] = 1.f;

	//Animation of jumping
	_scaleJump = new float[10];
	_frameScaleJump = 9.f;
	_scaleJump[9] = 1.f;

	//IntRect informations
	_borderTopLeft = Vector2i(0, 0);
	_borderBottomRight = Vector2i(width, height);

	//If multiple textures
	if (textures.size() > 0)
		_nbAnimationsPerTile = textures[0]->getSize().y / height;

	//Start animation
	PerformAnimation(_deltaTime);
}

void animation::ReverseSprite(bool reversed)
{
    _reversed = reversed;
}

void animation::SetLoop(int loop)
{
    _loop = loop;
}

int animation::GetLoop()
{
	return _loop;
}

void animation::SetAnimation(int numAnimation, int loop, float frame)
{
    if(numAnimation != _animation && numAnimation >= 0 && numAnimation < _nbAnimations)
    {
		_frame = frame > 0.f ? frame : 0.f;
        if(numAnimation != _staticAnimation)
            _staticFrame = 3.f;
        else if(numAnimation == _staticAnimation && _staticFrame > 0.f)
        {
            _staticFrame = _staticFrame > _deltaTime ? _staticFrame - _deltaTime : 0.f;
            return;
        }
    }
    else
        return;

    _loop = (loop >= 0 && loop < _nbFrames) ? loop : 0;
    _animation = (numAnimation >= 0 && numAnimation < _nbAnimations) ? numAnimation : 0;
}

void animation::SetFrameDelay(float delay)
{
    if(delay >= 0)
        _frameDelay = delay;
}

float animation::GetFrameDelay()
{
	return _frameDelay;
}

int animation::GetAnimation()
{
    return _animation;
}

int animation::GetNumTile(int numAnimation)
{
	int numTile = (int)((float)(numAnimation) / (float)_nbAnimationsPerTile);
	return _textures[numTile] != NULL ? numTile : 0;
}

int animation::GetTextureNum()
{
	if (_textures.size() > 0)
		return GetNumTile(_animation);
	else
		return 0;
}

float animation::GetFrame()
{
    return _frame;
}

int animation::GetNbFrames()
{
	return _nbFrames;
}

bool animation::GetReversed()
{
    return _reversed;
}

bool animation::isLastFrame()
{
    return ((int)_frame >= _nbFrames);
}

void animation::PerformFallingScale(float minScale)
{
	//Calculating frames
	minScale = minScale >= 0.f && minScale < 1.f ? minScale : 0.5;
	float step = (1.f - minScale) / 5.f;
	for (int i = 0; i<5; i++)
		_scaleFall[i] = 1.f - (((float)(i + 1))*step);
	for (int i = 5; i<10; i++)
		_scaleFall[i] = minScale + (((float)(i - 4))*step);

	//Init
	_frameScaleFall = 0.f;
}

void animation::PerformJumpingScale(float maxScale)
{
	//Calculating frames
	float step = (maxScale - 1.f) / 5.f;
	for (int i = 0; i<5; i++)
		_scaleJump[i] = 1.f + (((float)(i + 1))*step);
	for (int i = 5; i<10; i++)
		_scaleJump[i] = maxScale - (((float)(i - 4))*step);

	//Init
	_frameScaleJump = 0.f;
}

Vector2f animation::GetScale()
{
	return _tile->getScale();
}

void animation::CancelJumpFallScale()
{
	_frameScaleFall = 9.f;
	_frameScaleJump = 9.f;
}

void animation::PerformAnimation(float deltaTime)
{
    try
    {
        _deltaTime = deltaTime;

        //Reverse
        if(_reversed && !_disableScale)
            _tile->setScale(-1.f, _tile->getScale().y);
        else if (!_disableScale)
            _tile->setScale(1.f, _tile->getScale().y);

        //Increasing frame position
		if ((int)_frame >= _nbFrames)
			_frame = (float)_loop;
        else
            _frame += _frameDelay*_deltaTime;

        //Changing border top left
		int animationTile = _animation;
		if (_nbAnimations > _nbAnimationsPerTile && _textures.size() > 0)
		{
			int numTile = GetNumTile(_animation);
			_tile->setTexture(*_textures[numTile]);
			animationTile = _animation - (_nbAnimationsPerTile*numTile);
		}
        _borderTopLeft = Vector2i((int)_frame*_width, animationTile*_height);

        //Changing sprite rect
        if(_borderTopLeft.x + _borderBottomRight.x <= _tile->getTexture()->getSize().x &&
           _borderTopLeft.y + _borderBottomRight.y <= _tile->getTexture()->getSize().y)
            _tile->setTextureRect(IntRect(_borderTopLeft, _borderBottomRight));

        //Animation of falling / Jumping - crush sprite
        if(!_disableScale)
        {
			float scaleY = _frameScaleFall != 9.f ? _scaleFall[(int)_frameScaleFall] : _scaleJump[(int)_frameScaleJump];
			float signe = _frameScaleFall != 9.f ? 1.f : -1.f;
			float scaleX = (1.f + ((1.f - scaleY)*signe)) * (_reversed ? -1.f : 1.f);
            _tile->setScale(scaleX, scaleY);
			_frameScaleFall = utils::StepCooldown(_frameScaleFall, 1.f, _deltaTime, 9.f);
			_frameScaleJump = utils::StepCooldown(_frameScaleJump, 1.f, _deltaTime, 9.f);
        }
    }
    catch(exception)
    {
        cout << "Unable to perform animation." << endl;
    }
}

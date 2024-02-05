#ifndef animation_hpp
#define animation_hpp

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "../tools/utils.hpp"

using namespace std;
using namespace sf;

class animation
{
public:
	animation();
	~animation();
    animation(Sprite* tile, float frameDelay, int width, int height, int nbFrames, int nbAnimations, 
			  int staticAnimation, bool disableScale = false, vector<Texture*> textures = vector<Texture*>());
	void Reset(Sprite* tile, float frameDelay, int width, int height, int nbFrames, int nbAnimations,
		       int staticAnimation, bool disableScale = false, vector<Texture*> textures = vector<Texture*>());
	void ReverseSprite(bool reversed);
    void SetLoop(int loop);
	int GetLoop();
    void SetAnimation(int numAnimation, int loop, float frame = -1.f);
    void SetFrameDelay(float delay);
	float GetFrameDelay();
    int GetAnimation();
	int GetNumTile(int numAnimation);
	int GetTextureNum();
	float GetFrame();
	int GetNbFrames();
	bool GetReversed();
    bool isLastFrame();
	void PerformFallingScale(float minScale = 0.5f);
    void PerformJumpingScale(float maxScale = 1.25f);
	Vector2f GetScale();
	void CancelJumpFallScale();
    void PerformAnimation(float deltaTime);
private:
    Sprite* _tile;
	vector<Texture*> _textures;
    int _width;
    int _height;
    int _nbFrames;
    int _nbAnimations;
	int _nbAnimationsPerTile;
    int _animation;
    int _loop;
	int _staticAnimation;
    float _frame;
	float _frameDelay;
    float _deltaTime;
	float _staticFrame; //Avoid to set the first frame when changing direction
	float _frameScaleFall;
	float _frameScaleJump;
	float* _scaleFall;
	float* _scaleJump;
	bool _reversed;
    bool _disableScale;

    Vector2i _borderTopLeft;
    Vector2i _borderBottomRight;
};
#endif

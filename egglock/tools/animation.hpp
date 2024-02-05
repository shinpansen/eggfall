#ifndef animation_hpp
#define animation_hpp

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

using namespace std;
using namespace sf;

class animation
{
public:
    animation();
    animation(Sprite* tile, float frameDelay, int width, int height, int nbFrames, int nbAnimations,
              int staticAnimation, bool disableScale = false);
    void ReverseSprite(bool reversed);
    void SetLoop(int loop);
    void SetAnimation(int numAnimation, int loop);
    void SetFrameDelay(float delay);
    int GetAnimation();
    double GetFrame();
    bool GetReversed();
    bool isLastFrame();
    void PerformFallingScale();
    void PerformJumpingScale();
    void PerformAnimation(float deltaTime);
private:
    Sprite* _tile;
    int _width;
    int _height;
    int _nbFrames;
    int _nbAnimations;
    int _animation;
    int _loop;
    double _frame;
    double _frameDelay;
    float _deltaTime;
    bool _reversed;
    bool _disableScale;

    int _staticFrame, _staticAnimation; //Avoid to set the first frame when changing direction
    int _frameScaleFall;
    int _frameScaleJump;
    float* _scaleFall;
    float* _scaleJump;

    Vector2i _borderTopLeft;
    Vector2i _borderBottomRight;
};
#endif

#ifndef menuPad_HPP
#define menuPad_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../tools/xmlReader.hpp"
#include "../tools/files.hpp"
#include "../tools/input.hpp"

using namespace std;
using namespace sf;

class menuPad
{
public:
    menuPad();
    menuPad(int posY, string language, input* input);
    void ResetAnimation();
    void PerformBackText(RenderWindow* window);
	bool GetBackgroundVisibility();
	void SetPosition(int posY);
	void ShowBackground(bool show);
    void Display(RenderWindow* window, float deltaTime);
    bool _backTextFocused;
protected:
	//Variables - objects
    float _deltaTime;
    int _posY;
	bool _showBackground;
    string _language;
    input* _input;

	//Graphics
	Texture _pad360Texture;
	Sprite _pad360;
	RectangleShape _padShape;
	RectangleShape _background;
    Font _font;
    vector<Text*> _padTexts;
    Text _backText;
};
#endif

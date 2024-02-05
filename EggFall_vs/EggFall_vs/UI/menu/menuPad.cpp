#include "menuPad.hpp"

using namespace std;
using namespace sf;

menuPad::menuPad()
{
}

menuPad::menuPad(int posY, string language, input* input)
{
    _deltaTime = 1;
    _posY = posY;
    _language = language;
    _input = input;
    _backTextFocused = false;
	_showBackground = false;

    //Graphics
    _pad360Texture.loadFromFile(files::pad360);
    _pad360.setTexture(_pad360Texture);
    _padShape.setSize(Vector2f(utils::VIEW_WIDTH,250));
    _padShape.setFillColor(Color(20,20,20));
    _padShape.setOutlineColor(Color(40,40,40));
    _padShape.setOutlineThickness(5);
    _padShape.setOrigin(utils::VIEW_WIDTH/2.f,125);
    _padShape.setPosition(utils::VIEW_WIDTH / 2.f, posY+15);
    _padShape.setScale(1,0);

	//Back Text
	_font.loadFromFile(files::font2);
	_backText.setFont(_font);
	_backText.setString(xmlReader::getTranslationText("key2", _language, "BACK"));
	_backText.setCharacterSize(40);
	_backText.setOrigin(_backText.getLocalBounds().width / 2, _backText.getLocalBounds().height / 2 + 10);
	_backText.setPosition(utils::VIEW_WIDTH / 2.f, 600);

    //Position - Menu animation
	SetPosition(posY);
    ResetAnimation();
}


void menuPad::ResetAnimation()
{
    _padShape.setScale(1,0);
	_background.setSize(Vector2f(utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
	_background.setOrigin(utils::VIEW_WIDTH / 2.f, utils::VIEW_HEIGHT / 2.f);
	_background.setPosition(utils::VIEW_WIDTH / 2.f, utils::VIEW_HEIGHT / 2.f);
	_background.setFillColor(Color(0, 0, 0, 200));
	_background.setScale(0, 1);
}

void menuPad::PerformBackText(RenderWindow* window)
{
    Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    FloatRect textRect = _backText.getGlobalBounds();
    if(textRect.contains(mousePos))
    {
        _backTextFocused = true;
        _backText.setFillColor(Color(255,205,0));
    }
    else
    {
        _backTextFocused = false;
        _backText.setFillColor(Color(255,255,255));
    }
}

bool menuPad::GetBackgroundVisibility()
{
	return _showBackground;
}

void menuPad::SetPosition(int posY)
{
	//Texts
	_posY = posY;
	int maxLeft = 0, maxRight = 0;
	vector<string> padTextsStrings;
	padTextsStrings.push_back(xmlReader::getTranslationText("key", _language, "DASH"));
	padTextsStrings.push_back(xmlReader::getTranslationText("key", _language, "COUNTERACT") + "-" +
		xmlReader::getTranslationText("key2", _language, "BACK"));
	padTextsStrings.push_back(xmlReader::getTranslationText("key", _language, "JUMP") + "-" +
		xmlReader::getTranslationText("key2", _language, "VALID"));
	padTextsStrings.push_back(xmlReader::getTranslationText("key", _language, "ATTACK"));
	padTextsStrings.push_back(xmlReader::getTranslationText("key2", _language, "PAUSE"));
	padTextsStrings.push_back(xmlReader::getTranslationText("key2", _language, "MOVE"));
	for (int i = 0; i<padTextsStrings.size(); i++)
	{
		_padTexts.push_back(new Text());
		_padTexts[i]->setFont(_font);
		_padTexts[i]->setString(padTextsStrings[i]);
		_padTexts[i]->setCharacterSize(30);
		FloatRect textRect = _padTexts[i]->getLocalBounds();
		if (i <= 3)
		{
			_padTexts[i]->setOrigin(0, textRect.height + 5);
			if (textRect.width > maxRight) { maxRight = textRect.width; }
		}
		else
		{
			_padTexts[i]->setOrigin(textRect.width, textRect.height + 5);
			if (textRect.width > maxLeft) { maxLeft = textRect.width; }
		}
	}

	//Pad + texts width
	int width = utils::VIEW_WIDTH;
	maxLeft = 0;
	maxRight = 0;
	int padWidth = maxLeft + maxRight + _pad360.getLocalBounds().width;
	_pad360.setOrigin(-maxLeft, _pad360.getLocalBounds().height / 2);
	_pad360.setPosition((width - padWidth) / 2, posY);
	_padTexts[0]->setPosition(maxLeft + _pad360.getLocalBounds().width + (width - padWidth) / 2 + 5, posY - 85);
	_padTexts[1]->setPosition(maxLeft + _pad360.getLocalBounds().width + (width - padWidth) / 2 + 5, posY - 13);
	_padTexts[2]->setPosition(maxLeft + _pad360.getLocalBounds().width + (width - padWidth) / 2 + 5, posY + 22);
	_padTexts[3]->setPosition(maxLeft + _pad360.getLocalBounds().width + (width - padWidth) / 2 + 5, posY + 55);
	_padTexts[4]->setPosition(maxLeft + (width - padWidth) / 2 - 5, posY - 85);
	_padTexts[5]->setPosition(maxLeft + (width - padWidth) / 2 - 5, posY + 10);
}

void menuPad::ShowBackground(bool show)
{
	_showBackground = show;
}

void menuPad::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;

	//Background
	if (_showBackground)
		window->draw(_background);
	if (!_showBackground)
		window->draw(_padShape);

	//Transition
	if (_padShape.getScale().y < 1)
	{
		_padShape.setScale(1, _padShape.getScale().y + 0.1*_deltaTime);
		_background.setScale(_background.getScale().x + (0.1*_deltaTime), 1);
	}
    else //Transition finished
    {
		_padShape.setScale(1, 1);
		_background.setScale(1, 1);

        PerformBackText(window);
        window->draw(_pad360);
        for(int i=0 ; i<_padTexts.size() ; i++)
            window->draw(*_padTexts[i]);
        if(_input->GetMouseController())
            window->draw(_backText);
    }
}

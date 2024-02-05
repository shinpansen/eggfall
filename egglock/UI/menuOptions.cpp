#include "menuOptions.hpp"

using namespace std;
using namespace sf;

void menuOptions::UpdateOptions(vector<string> options, bool isKeys)
{
    _options = options;

    FloatRect textRect;
    int maxOptionSize = 0;
    for(int i=0 ; i<_options.size() ; i++)
    {
        Keyboard::Key key;
        if(isKeys)
            key = (Keyboard::Key)atoi(_options[i].c_str());
        int id = i+_maxItemIndex+1;
        if(id >= _texts.size())
            _texts.push_back(new Text());

        _texts[id]->setFont(_font);
        _texts[id]->setString(isKeys ? _input->GetKeyName(key) : _options[i]);
        _texts[id]->setCharacterSize(36);
        _texts[id]->setColor(Color(207,0,205));
        textRect = _texts[id]->getLocalBounds();
        _texts[id]->setOrigin(0, textRect.height/2+5);
        if(textRect.width > maxOptionSize)
            maxOptionSize = textRect.width;

        Vector2f pos = Vector2f(_texts[i]->getPosition().x + 40, _texts[i]->getPosition().y);
        _texts[id]->setPosition(pos);
    }


    //Shape
    _shape.setSize(Vector2f(_maxTextWidth + maxOptionSize + 110, _shape.getSize().y));
    ApplyMenuOutlines();
    _shape.setPosition(_pos.x - (maxOptionSize+50)/2, _shape.getPosition().y);

    for(int i=0 ; i<_texts.size() ; i++)
    {
        if (i <= _maxItemIndex)
            _texts[i]->setPosition(_pos.x + _maxTextWidth/2 - (maxOptionSize+50)/2, _texts[i]->getPosition().y);
        else
            _texts[i]->setPosition(_texts[i-_maxItemIndex-1]->getPosition().x + 40, _texts[i]->getPosition().y);
    }
}

void menuOptions::SetTextOption(int i, string text)
{
    _texts[i+_maxItemIndex+1]->setString(text);
}

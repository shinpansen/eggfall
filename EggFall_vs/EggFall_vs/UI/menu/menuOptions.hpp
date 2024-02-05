#ifndef menuOptions_HPP
#define menuOptions_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "menuList.hpp"

using namespace std;
using namespace sf;

class menuOptions : public menuList
{
public:
    menuOptions(vector<string> items, Vector2f pos, input* input, bool showBackground) :
    menuList(items, pos, input, showBackground, ALIGN_RIGHT){};
    void UpdateOptions(vector<string> options, bool isKeys);
    void SetTextOption(int i, string text);
private:
    vector<string> _options;
};
#endif

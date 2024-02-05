#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

#include "tools/Box2DTools.hpp"
#include "tools/input.hpp"
#include "tools/camera.hpp"
#include "tools/xmlReader.hpp"
#include "tools/settings.hpp"
#include "effects/effects.hpp"
#include "effects/spells.hpp"
#include "effects/sound.hpp"
#include "characters/hero.hpp"
#include "environment/maps.hpp"
#include "environment/tree.hpp"
#include "objects/ladder.hpp"
#include "UI/menuList.hpp"
#include "UI/mainGame.hpp"

using namespace sf;
using namespace std;

static const float SCALE = 30.f;

Image getTile(int row, int column, int tileSize);
Image printImage(Image source, Image tile, int x, int y);

int main(int argc, char **argv)
{
    //Window
    settings settings;
    Vector2i screenSize;
    if(settings._fullscreen == "ON")
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(800,600);
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    RenderWindow window(VideoMode(screenSize.x, screenSize.y, 32), "EggFall",
    settings._fullscreen == "ON" ? Style::Fullscreen : Style::Default, ctxSettings);

    //View
    FloatRect rect;
    if(600.0*screenSize.x/screenSize.y >= 800)
        rect = FloatRect(0,0,600.0*screenSize.x/screenSize.y,600);
    else
        rect = FloatRect(0,0,800,800*screenSize.y/screenSize.x);
    View view(rect); //800x600 native view
    view.setCenter(400,300);
    RectangleShape shapeLeft(Vector2f(500,600));
    RectangleShape shapeRight(Vector2f(500,600));
    RectangleShape shapeTop(Vector2f(800,200));
    RectangleShape shapeBottom(Vector2f(800,200));
    shapeLeft.setFillColor(Color(0,0,0));
    shapeRight.setFillColor(Color(0,0,0));
    shapeTop.setFillColor(Color(0,0,0));
    shapeBottom.setFillColor(Color(0,0,0));
    shapeLeft.setPosition(-500,0);
    shapeRight.setPosition(800,0);
    shapeTop.setPosition(0,-200);
    shapeBottom.setPosition(0,600);
    window.setView(view);

    /*Image img;
    img.create(504,1512,Color(0,0,0,0));
    for(int i=0 ; i<6 ; i++)
    {
        for(int j=0 ; j<18; j++)
        {
            Image tile = getTile(i,j,56);
            img = printImage(img, tile, (84*i)+14, (84*j)+28);
        }
    }
    img.saveToFile("E:\\egglock\\img.png");*/

    //THE GAME
    mainGame theGame(&window, &settings);
    Clock clock;
    float deltaTime;
    Keyboard::Key key;
    bool keyPressed;
    bool gamePaused = false;

    //Main loop
    static Event event;
    while (window.isOpen())
    {
        keyPressed = false;
        while (window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                   window.close();
                   break;
                case sf::Event::LostFocus:
                    gamePaused = true;
                    break;
                case sf::Event::GainedFocus:
                    gamePaused = false;
                    break;
                case sf::Event::KeyPressed:
                    key = event.key.code;
                    keyPressed = true;
                    break;
            }
        }

        if(!gamePaused)
        {
            window.clear(Color(35,31,32));

            //FPS - Adapting game speed to fps
            Time frameTime = clock.restart();
            deltaTime = 60*frameTime.asSeconds(); //60fps native speed

            //THE GAME CAN GO ON !
            theGame.GoOn(key, keyPressed, deltaTime);

            //Window properties
            window.draw(shapeLeft);
            window.draw(shapeRight);
            window.draw(shapeTop);
            window.draw(shapeBottom);
            window.setFramerateLimit(60);
            window.setVerticalSyncEnabled(true);
            window.display();
        }
        else
        {
            if(!theGame.IsMusicPaused())
                theGame.PauseMusic(true);
        }
    }

    return EXIT_SUCCESS;
}

Image getTile(int row, int column, int tileSize)
{
    files file;
    Image img;
    img.loadFromFile(file.wizardWhiteTile);
    Image tile;
    tile.create(tileSize, tileSize, Color(0,0,0,0));

    for(int i=tileSize*row; i<tileSize*(row+1) ; i++)
    {
        for(int j=tileSize*column; j<tileSize*(column+1) ; j++)
        {
            tile.setPixel(i-(tileSize*row), j-(tileSize*column), img.getPixel(i,j));
        }
    }

    return tile;
}

Image printImage(Image source, Image tile, int x, int y)
{
    for(int i=0; i<56 ; i++)
    {
        for(int j=0; j<56 ; j++)
        {
            source.setPixel(x+j, y+i, tile.getPixel(j,i));
        }
    }

    return source;
}

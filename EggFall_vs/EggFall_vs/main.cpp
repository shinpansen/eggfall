#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "tools/settings.hpp"
#include "UI/mainGame.hpp"

using namespace sf;
using namespace std;

static const float SCALE = 30.f;

Image getTile(int row, int column, int tileSize);
Image printImage(Image source, Image tile, int x, int y);

void StepGame(mainGame* theGame, bool * displayThreadBusy, bool * physicsThreadBusy)
{
    //Initialization
    Clock clock;
    float deltaTime = 1.0;
    float accumulator = 0.0;

    //Loop
    while (42 == 42) //because why not
    {
        //Updating delta time
        Time frameTime = clock.restart();
        deltaTime = 60.f*frameTime.asSeconds();
        accumulator += deltaTime;

        //Rendering frame
        if(accumulator >= 1.0 && !*displayThreadBusy)
        {
            *physicsThreadBusy = true;
            theGame->Render(1.0); // Physics and Inputs render
            accumulator = 0.0; //reset accumulator
            *physicsThreadBusy = false;
        }
    }
}

int main(int argc, char **argv)
{
	/*string filePath = "environment//mapFiles//mapTEST.xml";
	if (!files::FileExists(filePath))
		cout << "erreur";
	TiXmlDocument doc(filePath.c_str());
	doc.LoadFile();
	TiXmlElement* root = doc.FirstChildElement();
	for (TiXmlElement* root = doc.FirstChildElement(); root != NULL; root = root->NextSiblingElement())
	{
		cout << "# " << root->Value() << endl;
		for (TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
		{
			cout << elem->Value() << endl;
		}
	}*/

    //Window
    settings settings;
    Vector2i screenSize;
    if(settings._fullscreen == "ON")
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(utils::VIEW_WIDTH,utils::VIEW_HEIGHT);
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    RenderWindow window(VideoMode(screenSize.x, screenSize.y, 32), "EggFall",
    settings._fullscreen == "ON" ? Style::Fullscreen : Style::Default, ctxSettings);

    //The game
    mainGame theGame(&window, &settings);

    //Thread rendering physics at constant speed
	bool displayThreadBusy = false;
	bool physicsThreadBusy = false;
    float displayDeltaTime;
    Thread * physicsThread = new Thread(bind(&StepGame, &theGame, &displayThreadBusy, &physicsThreadBusy));
    physicsThread->launch();

    //View
    FloatRect rect;
    if(utils::VIEW_HEIGHT*screenSize.x/screenSize.y >= utils::VIEW_WIDTH)
        rect = FloatRect(0,0,utils::VIEW_HEIGHT*screenSize.x/screenSize.y, utils::VIEW_HEIGHT);
    else
        rect = FloatRect(0,0,utils::VIEW_WIDTH,utils::VIEW_WIDTH*screenSize.y/screenSize.x);
    View view(rect); //1280x720 native view
    view.setCenter(640, 360);
    RectangleShape shapeLeft(Vector2f(2000,2000));
    RectangleShape shapeRight(Vector2f(2000,2000));
    RectangleShape shapeTop(Vector2f(2000,2000));
    RectangleShape shapeBottom(Vector2f(2000,2000));
    shapeLeft.setFillColor(Color(0,0,0));
    shapeRight.setFillColor(Color(0,0,0));
    shapeTop.setFillColor(Color(0,0,0));
    shapeBottom.setFillColor(Color(0,0,0));
    shapeLeft.setPosition(-2000,-1000);
    shapeRight.setPosition(2000,-1000);
    shapeTop.setPosition(-420,-2000);
    shapeBottom.setPosition(-420,720);
    window.setView(view);
	//window.setFramerateLimit(60); //Bad idea
	//window.setVerticalSyncEnabled(true);

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

    //Rendering variables
    Clock clock;

    Keyboard::Key key = Keyboard::A; //Crash if not initialized
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
                   //physicsThread.terminate();
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
            //Clear window's content

			//Waiting for physics/input thread to avoid access violation errors
			/*while (physicsThreadBusy)
			{
				Time time;
				sleep(time);
			}*/

            //Delta time
            Time frameTime = clock.restart();
            displayDeltaTime = 60.f*frameTime.asSeconds(); //60fps based speed

			if (!physicsThreadBusy)
			{
				//Display the game
				displayThreadBusy = true;
				window.clear(Color(0, 0, 0));
				theGame.Display(&window, key, keyPressed, displayDeltaTime);
				displayThreadBusy = false;
			}

            //Window properties
            window.draw(shapeLeft);
            window.draw(shapeRight);
            window.draw(shapeTop);
            window.draw(shapeBottom);
            window.display();
        }
        else
        {
            if(!theGame.IsMusicPaused())
                theGame.PauseMusic(true);
        }
    }

    //Stopping thread
    physicsThread->terminate();

    //EVERYTHING RIGHT
    return EXIT_SUCCESS;
}

Image getTile(int row, int column, int tileSize)
{
    Image img;
    img.loadFromFile(files::wizardTextureA);
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

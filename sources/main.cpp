#include "../headers/dungeon.h"
#include "../headers/system.h"
#include "../headers/gamestate.h"

float mouseX, mouseY;
bool mousePressed, mouseClicked, mouseReleased;
const float massScale = 2.0f;

void playerControl(player &p)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        p.select(mouseX, mouseY);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        for (int i = 0; i < 1; ++i)
        {
            if (!p.selected[i])
                continue;
            // if (p.allies[i].visual == nullptr)
            //     continue;

            p.allies[i].MoveTo(mouseX,
                               mouseY);
        }
    }
}

float current_time = 0.0f;
float delta_time = 0.0f;
float previous_time = 0.0f;

void mouseUpdate()
{
    mouseClicked = false;
    mouseReleased = false;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressed)
    {
        mouseClicked = true;
        mousePressed = true;
    }

    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (mousePressed)
            mouseReleased = true;
        mouseClicked = false;
        mousePressed = false;
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(256, 128), "rambunctious_alpha_0.0");

    sprite violent("../img/classes/violent/brawler.png", 100.0f, 100.0f, massScale, massScale);
    // sf::Sprite s(textures[0]);

    room floor1("../img/tiles/walls/blue.png", "../img/tiles/floors/blue.png", massScale, massScale);
    character c1(&violent);
    player mainPlayer;
    mainPlayer.allies[0] = c1;
    mainPlayer.allies[0].walkToX = 120.0f;
    mainPlayer.allies[0].walkToY = 40.0f;

    sf::Clock sfClock;
    sf::Time sfTime;

    animation c1Idle(c1.visual, 0, 5, 150.0f);

    stategui.screenInitialization();

    while (window.isOpen())
    {
        mouseX = sf::Mouse::getPosition(window).x;
        mouseY = sf::Mouse::getPosition(window).y;

        previous_time = current_time;
        sfTime = sfClock.getElapsedTime();
        current_time = sfTime.asSeconds();
        delta_time = current_time - previous_time;

        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // playerControl(mainPlayer);
        mouseUpdate();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        window.clear();

        stategui.screenDraw(&window, mouseX, mouseY, mousePressed, mouseReleased);

        window.display();
    }

    return 0;
}
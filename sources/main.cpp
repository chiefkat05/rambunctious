#include "../headers/dungeon.h"
#include "../headers/system.h"
#include "../headers/gamestate.h"

float mouseX, mouseY;
bool mousePressed, mouseClicked, mouseReleased;
const float massScale = 2.0f;

void unitControl(player &p)
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

int prevState = -1;
// edit all guis here
void menuData()
{
    if (state == prevState)
        return;

    gui_data.buttons.clear();

    switch (state)
    {
    case START_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/start.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.buttons.push_back(button("../img/ui/buttons/start.png", 40.0f, 40.0f, 16.0f, 16.0f, startGame));
        break;
    case MENU_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/menu.png", 0.0f, 0.0f, 256.0f, 128.0f, 3, 1);
        gui_data.buttons.push_back(button("../img/ui/buttons/play.png", 40.0f, 40.0f, 16.0f, 16.0f, startGame));
        gui_data.buttons.push_back(button("../img/ui/buttons/quit.png", 80.0f, 40.0f, 16.0f, 16.0f, quitGame));
        break;
    case CHARACTER_CREATION_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/character.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        break;
    default:
        break;
    }

    gui_data.menuBG = animation(&gui_data.background, 0, gui_data.background.framesX - 1, 18.0f);

    prevState = state;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(256, 128), "rambunctious_alpha_0.0");

    sprite violent("../img/classes/violent/brawler.png", 100.0f, 100.0f, massScale, massScale, 5, 1);
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

        // unitControl(mainPlayer);
        mouseUpdate();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        window.clear();

        menuData();
        gui_data.screenDraw(&window, mouseX, mouseY, mousePressed, mouseReleased, delta_time);

        window.display();
    }

    return 0;
}
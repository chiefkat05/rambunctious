#include "../headers/dungeon.h"
#include "../headers/system.h"
#include "../headers/gamestate.h"

float mouseX, mouseY;
bool mousePressed, mouseClicked, mouseReleased;
const float massScale = 2.0f;

void unitControl(player &p)
{
    if (mouseClicked)
    {
        for (int i = 0; i < character_limit; ++i)
        {
            p.selected[i] = false;
        }
    }
    if (mousePressed)
    {
        p.select(mouseX, mouseY);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        int selectedCount = 0;
        float averageX = 0, averageY = 0;

        // two for-loops seems redundant, please optimize
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            averageX += p.allies[i].posX;
            averageY += p.allies[i].posY;
            ++selectedCount;
        }
        averageX /= selectedCount;
        averageY /= selectedCount;
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            float offsetX = p.allies[i].posX - averageX;
            float offsetY = p.allies[i].posY - averageY;

            p.allies[i].MoveTo(mouseX + offsetX,
                               mouseY + offsetY);
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
        gui_data.buttons.push_back(button("../img/ui/buttons/play.png", 120.0f, 120.0f, 16.0f, 16.0f, startGame));
        break;
    case DUNGEON_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/dungeon.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
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

    sprite violent("../img/classes/violent/brawler.png", 120.0f, 40.0f, massScale * 16.0f, massScale * 16.0f, 6, 1);
    sprite detective("../img/classes/violent/detective.png", 130.0f, 40.0f, massScale * 16.0f, massScale * 16.0f, 6, 1);
    sprite bloom("../img/classes/violent/bloom.png", 110.0f, 40.0f, massScale * 16.0f, massScale * 16.0f, 6, 1);
    // sf::Sprite s(textures[0]);

    room floor1("../img/tiles/walls/blue.png", "../img/tiles/floors/blue.png", massScale, massScale);
    character c1(&violent);
    character c2(&detective);
    character c3(&bloom);
    player mainPlayer;
    mainPlayer.allies[0] = c1;
    mainPlayer.allies[1] = c2;
    mainPlayer.allies[2] = c3;
    for (int i = 0; i < 3; ++i)
    {
        mainPlayer.allies[i].walkToX = mainPlayer.allies[i].posX;
        mainPlayer.allies[i].walkToY = mainPlayer.allies[i].posY;
    }

    sf::Clock sfClock;
    sf::Time sfTime;

    animation c1Idle(c1.visual, 0, 5, 150.0f);
    animation c2Idle(c2.visual, 0, 5, 150.0f);
    animation c3Idle(c3.visual, 0, 5, 150.0f);

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

        mouseUpdate();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        window.clear();

        menuData();
        gui_data.screenDraw(&window, mouseX, mouseY, mousePressed, mouseReleased, delta_time);
        if (state == DUNGEON_SCREEN)
        {
            unitControl(mainPlayer);

            floor1.draw(&window);
            for (int i = 0; i < character_limit; ++i)
            {
                if (mainPlayer.allies[i].visual == nullptr)
                {
                    continue;
                }
                window.draw(mainPlayer.allies[i].visual->rect);
                mainPlayer.allies[i].Update(delta_time);
                mainPlayer.allies[i].visual->Move(40.0f * delta_time, 0.0f);

                if (!mainPlayer.selected[i])
                {
                    if (mainPlayer.allies[i].visual != nullptr)
                        mainPlayer.allies[i].visual->rect.setColor(sf::Color(255, 255, 255, 255));
                    continue;
                }
                mainPlayer.allies[i].visual->rect.setColor(sf::Color(150, 150, 150, 100 + sin(current_time * 3.0f) * 70.0f));
            }
            c1Idle.run(delta_time);
            c2Idle.run(delta_time);
            c3Idle.run(delta_time);
        }

        window.display();
    }

    return 0;
}
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
#include <vector>

void startGame();
void optionsTab();
void quitGame();

sf::RenderWindow *win;

extern float massScale, massYOffset;

struct button
{
    sprite visual;
    float posX, posY, width, height;

    void (*onPressed)(int, player *);
    int func_n;
    player *func_p;

    button(sprite *v, float x, float y, float w, float h, void onP(int, player *), int f_n = 0, player *f_p = nullptr)
    {
        posX = x;
        posY = y;
        width = w;
        height = h;
        visual = *v;
        onPressed = onP;
        func_n = f_n;
        func_p = f_p;
    }
    button(const char *path, float x, float y, float w, float h, void onP(int, player *), int f_n = 0, player *f_p = nullptr)
    {
        posX = x;
        posY = y;
        width = w;
        height = h;
        visual = sprite(path, x, y, w, h, 1, 1);
        onPressed = onP;
        func_n = f_n;
        func_p = f_p;
    }

    void update(float mouseX, float mouseY, bool mousePressed, bool mouseReleased)
    {
        if (mouseX < posX / massScale || mouseX > posX / massScale + width / massScale ||
            mouseY < posY / massScale + massYOffset / massScale || mouseY > posY / massScale + height / massScale + massYOffset / massScale)
        {
            if (visual.rect.getColor().r < 255)
            {
                visual.rect.setColor(sf::Color(255, 255, 255, 255));
            }
            return;
        }

        if (!mousePressed)
            visual.rect.setColor(sf::Color(150, 150, 150, 255));
        else
            visual.rect.setColor(sf::Color(70, 70, 70, 255));

        if (!mouseReleased)
            return;

        visual.rect.setColor(sf::Color(255, 255, 255, 255));

        onPressed(func_n, func_p);
    }
};

enum game_state
{
    START_SCREEN,
    MENU_SCREEN,
    CHARACTER_CREATION_SCREEN,
    DUNGEON_SCREEN,
    state_total_count
};

game_state state = START_SCREEN;

struct gui
{
    std::vector<button> buttons;
    std::vector<sprite> images;
    sprite background;
    animation menuBG;
    bool quit = false;

    void screenDraw(sf::RenderWindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
    {
        if (quit)
            window->close();

        window->draw(background.rect);

        for (int i = 0; i < images.size(); ++i)
        {
            window->draw(images[i].rect);
        }
        for (int i = 0; i < buttons.size(); ++i)
        {
            window->draw(buttons[i].visual.rect);
            buttons[i].update(mouseX, mouseY, mousePressed, mouseReleased);
        }
        menuBG.run(delta_time, true);
    }
};

gui gui_data;

void startGame(int n, player *p)
{
    if (state > CHARACTER_CREATION_SCREEN)
        return;

    state = static_cast<game_state>(state + 1);

    // stategui.screenInitialization();
}
void optionsTab(int n, player *p)
{
}
void characterMenu(int allySelected, player *player)
{
    player->selected[allySelected] = true;
}
void quitGame(int n, player *p)
{
    gui_data.quit = true;
}

#endif
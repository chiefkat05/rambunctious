#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
#include <vector>

void startGame();
void optionsTab();
void quitGame();

sf::RenderWindow *win;

extern float massScale, massYOffset;

enum ui_element_type
{
    UI_CLICKABLE,
    UI_VALUEISFRAME,
    UI_IMAGE
};
struct ui_element
{
    sprite visual;
    float posX, posY, width, height;
    int *value;

    void (*function)(int, player *);
    int func_n;
    player *func_p;

    ui_element_type utype;
    animation anim;

    ui_element(ui_element_type t, sprite *v, float x, float y, float w, float h, void func(int, player *), int f_n = 0,
               player *f_p = nullptr, int *_linkValue = nullptr)
        : visual(*v), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
    {
        utype = t;
        posX = x;
        posY = y;
        width = w;
        height = h;
        function = func;
        func_n = f_n;
        func_p = f_p;
        value = _linkValue;
    }
    ui_element(ui_element_type t, const char *path, float x, float y, float w, float h, void func(int, player *), int f_n = 0,
               player *f_p = nullptr, int *_linkValue = nullptr)
        : visual(path, x, y, w, h, 1, 1), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
    {
        utype = t;
        posX = x;
        posY = y;
        width = w;
        height = h;
        function = func;
        func_n = f_n;
        func_p = f_p;
        value = _linkValue;
    }

    void update(float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
    {
        switch (utype)
        {
        case UI_CLICKABLE:
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

            function(func_n, func_p);
            break;
        case UI_VALUEISFRAME:
            anim._sprite = &visual; // this should only happen once or so
            anim.frame = *value;
            anim.timer = 1.0f;
            anim.run(delta_time, false);
        default:
            break;
        }
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
    std::vector<ui_element> elements;
    sprite background;
    animation menuBG;
    bool quit = false;

    void screenDraw(sf::RenderWindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
    {
        if (quit)
            window->close();

        window->draw(background.rect);

        for (int i = 0; i < elements.size(); ++i)
        {
            elements[i].update(mouseX, mouseY, mousePressed, mouseReleased, delta_time);
            window->draw(elements[i].visual.rect);
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
void nullFunc(int n, player *p) {}
void characterMenu(int allySelected, player *player)
{
    player->selected[allySelected] = true;
}
void quitGame(int n, player *p)
{
    gui_data.quit = true;
}

#endif
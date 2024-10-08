#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
#include <vector>

void startGame();
void optionsTab();
void quitGame();

sf::RenderWindow *win;

extern float massScale, massYOffset;
extern bool buttonHovered;

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

    void (*function)(player *, dungeon *, int);
    player *func_p;
    dungeon *func_d;
    int func_i;

    ui_element_type utype;
    animation anim;

    ui_element(ui_element_type t, sprite *v, float x, float y, float w, float h, void func(player *, dungeon *, int), player *_func_p = nullptr, dungeon *_func_d = nullptr,
               int _func_i = 0, int *_linkValue = nullptr)
        : visual(*v), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
    {
        utype = t;
        posX = x;
        posY = y;
        width = w;
        height = h;
        function = func;
        func_p = _func_p;
        func_d = _func_d;
        func_i = _func_i;
        value = _linkValue;
    }
    ui_element(ui_element_type t, const char *path, float x, float y, float w, float h, int frX, int frY, void func(player *, dungeon *, int),
               player *_func_p = nullptr, dungeon *_func_d = nullptr,
               int _func_i = 0, int *_linkValue = nullptr)
        : visual(path, x, y, w, h, frX, frY), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
    {
        utype = t;
        posX = x;
        posY = y;
        width = w;
        height = h;
        function = func;
        func_p = _func_p;
        func_d = _func_d;
        func_i = _func_i;
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
            buttonHovered = true;

            if (!mousePressed)
                visual.rect.setColor(sf::Color(150, 150, 150, 255));
            else
                visual.rect.setColor(sf::Color(70, 70, 70, 255));

            if (!mouseReleased)
                return;

            visual.rect.setColor(sf::Color(255, 255, 255, 255));

            function(func_p, func_d, func_i);
            break;
        case UI_VALUEISFRAME:
            if (value == nullptr)
                break;
            anim._sprite = &visual; // this should only happen once or so
            anim.frame = *value;
            anim.timer = 1.0f;
            anim.run(delta_time, false);
            break;
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
    LOSE_SCREEN,
    WIN_SCREEN,
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

void startGame(player *p, dungeon *d, int argv)
{
    if (state > CHARACTER_CREATION_SCREEN)
        return;

    state = static_cast<game_state>(state + 1);

    // stategui.screenInitialization();
}
void optionsTab(player *p, dungeon *d, int argv)
{
}
void nullFunc(player *p, dungeon *d, int argv) {}
void characterMenu(player *player, dungeon *d, int argv)
{
    for (int i = 0; i < character_limit; ++i)
    {
        if (player->allies[i].visual == nullptr)
            continue;

        player->selected[i] = false;
        player->allies[i].visual->rect.setColor(sf::Color(255, 255, 255, 255));
    }
    player->selected[argv] = true;
    player->allies[argv].visual->rect.setColor(sf::Color(255, 255, 255, 170));
}
void gotoCharacter(player *player, dungeon *floor, int argv)
{
    floor->screenPositionX = -player->allies[argv].posX + 128.0f;
    floor->screenPositionY = -player->allies[argv].posY + 64.0f;
    // floor->screenPositionY = player->allies[argv].posY;
}

void characterAbility(player *player, dungeon *d, int argv)
{
    int selection = -1;
    for (int i = 0; i < character_limit; ++i)
    {
        if (player->selected[i])
        {
            selection = i;
            break;
        }
    }
    if (selection == -1)
        return;

    player->allies[selection].nextAbility = argv;
}
void quitGame(player *p, dungeon *d, int argv)
{
    gui_data.quit = true;
}

#endif
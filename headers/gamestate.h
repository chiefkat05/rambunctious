#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
#include <vector>

void startGame();
void optionsTab();
void quitGame();

sf::RenderWindow *win;

struct button
{
    sprite visual;
    float posX, posY, width, height;
    void (*onPressed)();

    button(sprite *v, float x, float y, float w, float h, void onP())
    {
        posX = x;
        posY = y;
        width = w;
        height = h;
        visual = *v;
        onPressed = onP;
    }
    button(const char *path, float x, float y, float w, float h, void onP())
    {
        posX = x;
        posY = y;
        width = w;
        height = h;
        visual = sprite(path, x, y, w, h, 1, 1);
        onPressed = onP;
    }

    void update(float mouseX, float mouseY, bool mousePressed, bool mouseReleased)
    {
        if (mouseX < posX || mouseX > posX + width || mouseY < posY || mouseY > posY + height)
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

        onPressed();
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
    sprite background;
    animation menuBG;
    bool quit = false;

    //     case DUNGEON_SCREEN:
    //         // mainPlayer.allies[0].Update(delta_time);
    //         // if (mainPlayer.selected[0])
    //         // {
    //         //     mainPlayer.allies[0].visual.rect.setColor(sf::Color(150, 150, 150, 100 + sin(current_time * 3.0f) * 70.0f));
    //         // }
    //         // if (!mainPlayer.selected[0])
    //         // {
    //         //     mainPlayer.allies[0].visual.rect.setColor(sf::Color(255, 255, 255, 255));
    //         // }

    //         // violent.Move(40.0f * delta_time, 0.0f);
    //         // c1Idle.run(delta_time);

    //         // floor1.draw(&window);
    //         // window.draw(mainPlayer.allies[0].visual.rect);
    //         break;
    //     default:
    //         break;
    //     }

    void screenDraw(sf::RenderWindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
    {
        if (quit)
            window->close();

        window->draw(background.rect);

        for (int i = 0; i < buttons.size(); ++i)
        {
            window->draw(buttons[i].visual.rect);
            buttons[i].update(mouseX, mouseY, mousePressed, mouseReleased);
        }
        menuBG.run(delta_time);
    }
};

gui gui_data;

void startGame()
{
    if (state > CHARACTER_CREATION_SCREEN)
        return;

    state = static_cast<game_state>(state + 1);

    // stategui.screenInitialization();
}
void optionsTab()
{
}
void quitGame()
{
    gui_data.quit = true;
}

#endif
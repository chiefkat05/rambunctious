#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
#include <vector>

void startGame();

struct button
{
    sprite *visual;
    float posX, posY, width, height;

    button(sprite *v, float x, float y, float w, float h)
    {
        posX = x;
        posY = y;
        width = w;
        height = h;
        visual = v;
    }

    void update(float mouseX, float mouseY, bool mousePressed, bool mouseReleased, void onPressed())
    {
        if (mouseX < posX || mouseX > posX + width || mouseY < posY || mouseY > posY + height)
        {
            if (visual->rect.getColor().r < 255)
            {
                visual->rect.setColor(sf::Color(255, 255, 255, 255));
            }
            return;
        }

        if (!mousePressed)
            visual->rect.setColor(sf::Color(150, 150, 150, 255));
        else
            visual->rect.setColor(sf::Color(70, 70, 70, 255));

        if (!mouseReleased)
            return;

        visual->rect.setColor(sf::Color(255, 255, 255, 255));

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
    std::vector<sprite> images;
    sprite background;

    void screenInitialization()
    {
        switch (state)
        {
        case START_SCREEN:
            background = sprite("../img/ui/backgrounds/start.png", 0.0f, 0.0f, 1.0f, 1.0f); // make a sick placeholder background for this
            images.push_back(sprite("../img/ui/buttons/start.png", 100.0f, 100.0f, 1.0f, 1.0f));
            buttons.push_back(button(&images[0], 100.0f, 100.0f, 16.0f, 16.0f));
            break;
        case DUNGEON_SCREEN:
            // mainPlayer.allies[0].Update(delta_time);
            // if (mainPlayer.selected[0])
            // {
            //     mainPlayer.allies[0].visual->rect.setColor(sf::Color(150, 150, 150, 100 + sin(current_time * 3.0f) * 70.0f));
            // }
            // if (!mainPlayer.selected[0])
            // {
            //     mainPlayer.allies[0].visual->rect.setColor(sf::Color(255, 255, 255, 255));
            // }

            // // violent.Move(40.0f * delta_time, 0.0f);
            // c1Idle.run(delta_time);

            // floor1.draw(&window);
            // window.draw(mainPlayer.allies[0].visual->rect);
            break;
        default:
            break;
        }
    }

    void screenDraw(sf::RenderWindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased)
    {
        window->draw(background.rect);
        switch (state)
        {
        case START_SCREEN:
            for (int i = 0; i < buttons.size(); ++i)
            {
                window->draw(buttons[i].visual->rect);
                buttons[i].update(mouseX, mouseY, mousePressed, mouseReleased, startGame);
            }
            break;
        case DUNGEON_SCREEN:
            break;
        default:
            break;
        }
    }
};

gui stategui;

void startGame()
{
    if (state > CHARACTER_CREATION_SCREEN)
        return;

    state = static_cast<game_state>(state + 1);

    stategui.screenInitialization();
}

#endif
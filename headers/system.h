#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include "sprite.h"

const unsigned int att_limit = 4;
const unsigned int stat_limit = 4;
const unsigned int entity_limit = 64;
const unsigned int character_limit = 6;

enum STATUS
{
    BUFFED,
    NERFED,
    MINOR_WOUND,
    MAJOR_WOUND,
    DEAD
};

struct attack
{
    std::pair<STATUS, unsigned int> status_chances[stat_limit];
};

struct character
{
    attack abilities[att_limit];
    STATUS statuses[stat_limit];
    float posX = 0.0f, posY = 0.0f, selectionSize = 4.0f;
    float walkToX = 0.0f, walkToY = 0.0f;
    sprite *visual = nullptr;

    unsigned int initiative = 0;

    character() {}
    character(sprite *v) : visual(v)
    {
        visual->rect.setTextureRect(sf::IntRect(0, 0, 16, 16));
        visual->rect.setOrigin(sf::Vector2(8.0f, 16.0f));
        posX = visual->rect.getPosition().x;
        posY = visual->rect.getPosition().y;
    }

    void MoveTo(float _x, float _y)
    {
        walkToX = _x;
        walkToY = _y;
    }

    void Update(float delta_time)
    {
        // posX += 40.0f * delta_time;

        if (walkToX < posX)
            posX -= 120.0f * delta_time;
        if (walkToX > posX)
            posX += 120.0f * delta_time;
        if (walkToY < posY)
            posY -= 120.0f * delta_time;
        if (walkToY > posY)
            posY += 120.0f * delta_time;

        visual->Put(posX, posY);
    }
};

struct terrain
{
    std::pair<STATUS, unsigned int> status_chances[stat_limit];
};

struct system
{
    character characters[entity_limit];
    terrain ground, air;
};

struct player
{
    character allies[character_limit];
    bool selected[character_limit];

    void select(float mouseX, float mouseY)
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (mouseX > allies[i].posX - allies[i].selectionSize && mouseX < allies[i].posX + allies[i].selectionSize)
            {
                selected[i] = true;
            }
        }
    }
};

#endif
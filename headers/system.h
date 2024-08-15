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

enum IDENTIFICATION
{
    CH_PLAYER,
    CH_MONSTER,
    CH_EVIL,
    CH_NEUTRAL,
    CH_HERO,
    CH_COMMONER
};

struct character
{
    attack abilities[att_limit];
    STATUS statuses[stat_limit];
    float posX = 0.0f, posY = 0.0f, selectionSize = 4.0f;
    float walkToX = 0.0f, walkToY = 0.0f;
    sprite *visual = nullptr;

    character *target = nullptr;
    IDENTIFICATION id = CH_MONSTER;
    float attackSpeed = 10.0f, attackTimer = attackSpeed, runSpeed = 120.0f;
    int hp = 10, maxHP = 10, dmg = 4;

    unsigned int initiative = 0;

    character() {}
    character(sprite *v, IDENTIFICATION _id) : visual(v)
    {
        visual->rect.setTextureRect(sf::IntRect(0, 0, 16, 16));
        visual->rect.setOrigin(sf::Vector2(8.0f, 16.0f));
        posX = visual->rect.getPosition().x;
        posY = visual->rect.getPosition().y;
        walkToX = posX;
        walkToY = posY;
        id = _id;
    }

    void MoveTo(float _x, float _y)
    {
        walkToX = _x;
        walkToY = _y;
    }

    void strikeTarget(float delta_time)
    {
        attackTimer -= 10.0f * delta_time;

        if (attackTimer < 0.0f)
        {
            attackTimer = attackSpeed;
            target->hp -= dmg;
        }
    }

    void Update(float delta_time)
    {
        if (hp <= 0)
        {
            visual->rect.setColor(sf::Color(50, 50, 50, 255));
            visual->rect.setRotation(90);
            return;
        }

        if (target != nullptr)
        {
            walkToX = target->posX;
            walkToY = target->posY;

            float xDist = posX - target->posX;
            float yDist = posY - target->posY;

            float distance = xDist * xDist + yDist * yDist;

            if (distance > 4000.0f || target->hp <= 0)
            {
                target = nullptr;
            }
            if (distance < 400.0f)
            {
                strikeTarget(delta_time);
            }
        }

        float xWalkDist = posX - walkToX;
        float yWalkDist = posY - walkToY;

        if (walkToX != posX && walkToY == posY)
        {
            posX -= xWalkDist / std::abs(xWalkDist) * runSpeed * delta_time;
        }
        if (walkToY != posY && walkToX == posX)
        {
            posY -= yWalkDist / std::abs(yWalkDist) * runSpeed * delta_time;
        }
        if (walkToX != posX && walkToY != posY)
        {
            float normalizationCap = std::max(std::abs(xWalkDist), std::abs(yWalkDist));
            posX -= xWalkDist / normalizationCap * runSpeed * delta_time;
            posY -= yWalkDist / normalizationCap * runSpeed * delta_time;
        }

        visual->Put(posX, posY);
    }
};

struct terrain
{
    std::pair<STATUS, unsigned int> status_chances[stat_limit];
};

struct player
{
    character allies[character_limit];
    bool selected[character_limit];

    void select(float mouseX, float mouseY)
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (mouseX > allies[i].posX - allies[i].selectionSize && mouseX < allies[i].posX + allies[i].selectionSize &&
                mouseY > allies[i].posY - allies[i].selectionSize && mouseY < allies[i].posY + allies[i].selectionSize)
            {
                selected[i] = true;
            }
        }
    }
};

struct quadtree
{
    unsigned int min, max;

    quadtree *child;
};

struct game_system
{
    character *characters[entity_limit];
    int characterCount = 0;
    terrain ground, air;

    void Add(player *p)
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (p->allies[i].visual == nullptr)
                continue;

            characters[characterCount] = &p->allies[i];
            ++characterCount;
        }
    }
    void Add(character *e)
    {
        characters[characterCount] = e;
        ++characterCount;
    }

    void update(float delta_time)
    {
        for (int i = 0; i < characterCount; ++i)
        {
            for (int j = 0; j < characterCount; ++j)
            {
                if (i == j)
                    continue;

                switch (characters[i]->id)
                {
                case CH_PLAYER:
                    break;
                case CH_HERO:
                    break;
                case CH_COMMONER:
                    break;
                case CH_MONSTER:
                    if (characters[j]->id != CH_PLAYER && characters[j]->id != CH_COMMONER && characters[j]->id != CH_HERO)
                    {
                        break;
                    }

                    if (characters[i]->target == nullptr && characters[j]->hp > 0)
                    {
                        float xDist = characters[i]->posX - characters[j]->posX;
                        float yDist = characters[i]->posY - characters[j]->posY;

                        float distance = xDist * xDist + yDist * yDist;

                        if (distance < 4000.0f)
                        {
                            characters[i]->target = characters[j];
                        }
                    }
                    break;
                case CH_EVIL:
                    break;
                case CH_NEUTRAL:
                    break;
                }

                // if (target == nullptr && distance < 4000.0f && distance > 400.0f)
                // {
                //     target = &mainP.allies[i];
                // }
                // if (target == &mainP.allies[i] && distance > 4000.0f)
                // {
                //     target = nullptr;
                // }

                // if (target != nullptr)
                // {
                //     float xDist = unit.posX - target->posX;
                //     float yDist = unit.posY - target->posY;
                //     float normalizationCap = std::max(std::abs(xDist), std::abs(yDist));

                //     float distance = xDist * xDist + yDist * yDist;

                //     if (distance > 400.0f)
                //         unit.MoveTo(unit.posX - xDist / normalizationCap, unit.posY - yDist / normalizationCap);
                // }
            }

            characters[i]->Update(delta_time);
        }
    }
};

#endif
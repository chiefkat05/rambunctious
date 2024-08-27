#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include "sprite.h"

const unsigned int att_limit = 4;
const unsigned int class_att_limit = 52;
const unsigned int stat_limit = 14;
const unsigned int entity_limit = 64;
const unsigned int character_limit = 6;
const unsigned int animation_limit = 24;

extern float massScale, massYOffset;

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
    int dmg;
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

enum ANIMATION_MAPPINGS
{
    ANIM_IDLE,
    ANIM_WALK,
    ANIM_ATTACK,
    ANIM_HURT,
    ANIM_DEAD,
    ANIM_SPECIAL0 // continue with only specials, place hardcoded animations before the special animations (which are for custom scenarios)
};

struct ch_class
{
    attack abilities_list[class_att_limit];
    STATUS permanent_status_list[stat_limit];

    int maxHP;
    float attackSpeed, runSpeed, attackRange;

    float experienceToLvlUp;

    ch_class()
    {
        maxHP = 10;
        abilities_list[0].dmg = 5;
        attackSpeed = 10.0f;
        runSpeed = 100.0f;
        attackRange = 100.0f;
        experienceToLvlUp = 100;
    }

    ch_class(int _mhp, float _attspd, float _runspd, float _attrng, float _xpLvlUp)
        : maxHP(_mhp), attackSpeed(_attspd),
          runSpeed(_runspd), attackRange(_attrng),
          experienceToLvlUp(_xpLvlUp) {}
};

struct character
{
    ch_class _class;

    STATUS statuses[stat_limit];
    animation animations[animation_limit];
    float posX = 0.0f, posY = 0.0f, selectionSize = 12.0f;
    float walkToX = 0.0f, walkToY = 0.0f;
    sprite *visual = nullptr;

    character *target = nullptr;
    IDENTIFICATION id = CH_MONSTER;
    float attackTimer = 0.0f;
    int hp = 10;

    bool animationFinished = true, animationLooping = false;
    ANIMATION_MAPPINGS playingAnim = ANIM_IDLE;

    unsigned int initiative = 0;

    character() {}
    character(sprite *v, IDENTIFICATION _id, ch_class cl) : visual(v)
    {
        visual->rect.setTextureRect(sf::IntRect(0, 0, visual->spriteW, visual->spriteH));
        visual->rect.setOrigin(sf::Vector2(static_cast<float>(visual->spriteW) * 0.5f, static_cast<float>(visual->spriteH) * 0.5f));
        posX = visual->rect.getPosition().x;
        posY = visual->rect.getPosition().y;
        walkToX = posX;
        walkToY = posY;
        id = _id;
        _class = cl;
    }

    void LevelUp()
    {
        _class.maxHP *= 1.1f;
        _class.attackSpeed /= 1.1f;
        _class.experienceToLvlUp *= 1.5f;
    }

    void MoveTo(float _x, float _y)
    {
        walkToX = _x;
        walkToY = _y;
    }

    void takeHit(float delta_time)
    {
        PlayAnimation(ANIM_HURT, delta_time, false);
    }

    void strikeTarget(float delta_time)
    {
        attackTimer -= 10.0f * delta_time;

        if (attackTimer < 0.0f)
        {
            std::cout << _class.abilities_list[0].dmg << ", " << _class.attackSpeed << ", " << target->hp << " ????\n"; // what the frick is happening with the class stats
            attackTimer = _class.attackSpeed;
            target->hp -= _class.abilities_list[0].dmg;
            target->takeHit(delta_time);
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

            if (target->hp <= 0)
            {
                target = nullptr;
            }
            if (distance < _class.attackRange)
            {
                walkToX = posX;
                walkToY = posY;
                strikeTarget(delta_time);
            }
        }

        float xWalkDist = posX - walkToX;
        float yWalkDist = posY - walkToY;

        if (walkToX != posX && walkToY == posY)
        {
            posX -= xWalkDist / std::abs(xWalkDist) * _class.runSpeed * delta_time;
        }
        if (walkToY != posY && walkToX == posX)
        {
            posY -= yWalkDist / std::abs(yWalkDist) * _class.runSpeed * delta_time;
        }
        if (walkToX != posX && walkToY != posY)
        {
            float normalizationCap = std::max(std::abs(xWalkDist), std::abs(yWalkDist));
            posX -= xWalkDist / normalizationCap * _class.runSpeed * delta_time;
            posY -= yWalkDist / normalizationCap * _class.runSpeed * delta_time;
        }

        if (animationFinished)
        {
            PlayAnimation(ANIM_IDLE, delta_time, true);
        }

        animations[playingAnim].run(delta_time, animationLooping, &animationFinished);

        visual->Put(posX, posY);

        animationFinished = false;
    }

    void SetAnimation(ANIMATION_MAPPINGS id, unsigned int s, unsigned int e, float spd)
    {
        animations[id] = animation(visual, s, e, spd);
    }
    void PlayAnimation(ANIMATION_MAPPINGS id, float delta_time, bool loops)
    {
        if (animations[id]._sprite == nullptr)
            return;

        animations[id].frame = animations[id].start;
        animations[id].timer = 10.0f;
        playingAnim = id;
        animationLooping = loops;
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

    void select(float minX, float minY, float maxX, float maxY)
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (minX < allies[i].posX / massScale &&
                maxX > allies[i].posX / massScale &&
                minY < allies[i].posY / massScale + massYOffset &&
                maxY > allies[i].posY / massScale + massYOffset)
            {
                selected[i] = true;
            }
            else
            {
                selected[i] = false;
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
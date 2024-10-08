#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <string>
#include "sprite.h"
#include "collision.h"
#include "dungeon.h"

const unsigned int att_limit = 4;
const unsigned int class_att_limit = 52;
const unsigned int stat_limit = 14;
const unsigned int entity_limit = 64; // also change in dungeon.h since there's another one there, also delete one of these so that there's only one pls
const unsigned int character_limit = 6;
const unsigned int animation_limit = 24;
const unsigned int target_limit = 12;

extern float massScale, massYOffset;

enum STATUS
{
    BUFFED,      // any effect originating from an ally
    NERFED,      // any effect originating from an enemy
    MINOR_WOUND, // take over 1% of hp in one hit, less than 10%
    MAJOR_WOUND, // take over 10% of hp in one hit
    DEAD         // hp is 0 or less
};

struct character;

// animation. When animation is on x frame, trigger ability. Ability consists of a list of targets, and a function. The function will take the targets and do something with them.
struct attack
{
    int targetCount;
    int activationFrame;
    float distanceRequired;
    character *targets[target_limit];
    character *parent;
    void (*use)(int, attack *, character *);

    void clear()
    {
        for (int i = 0; i < target_limit; ++i)
        {
            targets[i] = nullptr;
        }
    }
    void updateTargets(character *tlist[target_limit], int tcount)
    {
        clear();

        for (int i = 0; i < tcount; ++i)
        {
            targets[i] = tlist[i];
        }
        targetCount = tcount;
    }
};

void ability_brawler_right_hook(int value, attack *thisAttack, character *parent);

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
    ANIM_HURT,
    ANIM_DEAD,
    ANIM_ABILITY_0, // continue with only specials, place hardcoded animations before the special animations (which are for custom scenarios)
    ANIM_ABILITY_1,
    ANIM_ABILITY_2,
    ANIM_ABILITY_3,
    ANIM_ABILITY_4,
    ANIM_ABILITY_5,
    ANIM_ABILITY_6,
    ANIM_ABILITY_7,
    ANIM_ABILITY_8
};

struct ch_class
{
    std::string name;
    attack abilities_list[att_limit];
    STATUS permanent_status_list[stat_limit];

    int maxHP, attackDamage;
    float attackSpeed, runSpeed, attackRange;

    int experienceToLvlUp;

    ch_class()
    {
        maxHP = 10;
        attackSpeed = 10.0f;
        runSpeed = 100.0f;
        attackRange = 100.0f;
        experienceToLvlUp = 100;
    }

    ch_class(std::string _n, int _mhp, int _attdmg, float _attspd, float _runspd, float _attrng, int _xpLvlUp, void ability_func(int, attack *, character *))
        : name(_n), maxHP(_mhp), attackSpeed(_attspd),
          runSpeed(_runspd), attackRange(_attrng),
          experienceToLvlUp(_xpLvlUp)
    {
        attackDamage = _attdmg;
        for (int i = 0; i < att_limit; ++i)
        {
            abilities_list[i].distanceRequired = _attrng;
            abilities_list[i].use = ability_func;
        }
    }

    void setAbility(int index, void _func(int, attack *, character *), float distanceMultiple = 1.0f, int _frame = 0)
    {
        abilities_list[index].use = _func;
        abilities_list[index].distanceRequired = attackRange * distanceMultiple;
        abilities_list[index].activationFrame = _frame;
    }
};

struct character
{
    ch_class _class;
    float velocityX = 0.0f, velocityY = 0.0f;
    int nextAbility = 0, lastAbility = 0;

    STATUS statuses[stat_limit];
    animation animations[animation_limit];
    float posX = 0.0f, posY = 0.0f, selectionSize = 12.0f;
    float walkToX = 0.0f, walkToY = 0.0f;
    sprite *visual = nullptr;
    aabb collider;

    character *target = nullptr;
    IDENTIFICATION id = CH_MONSTER;
    float attackTimer = 0.0f;
    int hp = 10;
    int experiencePoints = 0, level = 1;

    bool animationFinished = true, animationLooping = false;
    bool attacking = false;
    ANIMATION_MAPPINGS playingAnim = ANIM_IDLE;

    unsigned int initiative = 0;

    character() {}
    character(sprite *v, IDENTIFICATION _id, ch_class cl) : visual(v)
    {
        visual->rect.setTextureRect(sf::IntRect(0, 0, visual->spriteW, visual->spriteH)); // the brawler origin is off-center because he's 32x32 for animation purposes
        visual->rect.setOrigin(sf::Vector2(static_cast<float>(visual->spriteW) * 0.5f, static_cast<float>(visual->spriteH) * 0.5f));
        posX = visual->rect.getPosition().x;
        posY = visual->rect.getPosition().y;
        walkToX = posX;
        walkToY = posY;
        id = _id;
        _class = cl;
        hp = cl.maxHP;

        collider = aabb(posX, posY, posX + 16.0f, posY + 8.0f);
    } // pathfinding time also you can move everything into cpp files

    void LevelUp()
    {
        _class.maxHP *= 1.1f;
        _class.attackSpeed /= 1.1f;
        _class.experienceToLvlUp *= 1.5f;
        hp = _class.maxHP;
    }

    void MoveTo(float _x, float _y, dungeon *currentDungeon)
    {
        if (currentDungeon != nullptr)
        {
            _x -= currentDungeon->screenPositionX;
            _y -= currentDungeon->screenPositionY;
        }

        walkToX = _x;
        walkToY = _y;
    }

    void takeHit(float delta_time)
    {
        PlayAnimation(ANIM_HURT, delta_time, false);
    }

    void strikeTarget(float delta_time)
    {
        if (!attacking)
        {
            attackTimer -= 10.0f * delta_time;
        }

        if (attackTimer < 0.0f && playingAnim != ANIM_ABILITY_0 + nextAbility)
        {
            PlayAnimation(static_cast<ANIMATION_MAPPINGS>(ANIM_ABILITY_0 + nextAbility), delta_time, false);
            attacking = true;
        }
        if (attacking && animations[ANIM_ABILITY_0 + nextAbility].frame >= _class.abilities_list[nextAbility].activationFrame)
        {
            attackTimer = _class.attackSpeed;
            _class.abilities_list[nextAbility].updateTargets(new character *{target}, 1);
            _class.abilities_list[nextAbility].use(_class.attackDamage, &_class.abilities_list[0], this);
            target->takeHit(delta_time); // this needs to be a reference to the attack->targets, not positional target. Also, attack->targets needs to be able to target anybody within range of the attack, capped at attack->targetcount
            attacking = false;
            lastAbility = nextAbility;

            if (target->hp <= 0 && target->id != id)
            {
                experiencePoints += target->_class.maxHP / 10;
            }
        }
        // figure out when an ability should be used. Last ability should be updated when an ability is used (or one line before it's set so that you can set it to the ability that's about to be overridden).
        // There should be an overridable function (boolean?) that is checked once a move is used, and then that move will be set as the nextAbility and used next.
    }

    void Update(float delta_time, float screenOffsetX, float screenOffsetY)
    {
        collider.moveCenterToPoint(posX, posY + 4.0f);

        if (hp <= 0)
        {
            visual->rect.setColor(sf::Color(50, 50, 50, 255));
            visual->rect.setRotation(90);
            visual->Put(posX + screenOffsetX, posY + screenOffsetY);
            return;
        }

        if (target != nullptr)
        {

            float xDist = posX - target->posX;
            float yDist = posY - target->posY;

            float distance = xDist * xDist + yDist * yDist;

            if (distance > _class.attackRange)
            {
                walkToX = target->posX;
                walkToY = target->posY;
            }
            else
            {
                strikeTarget(delta_time);
            }

            if (target->hp <= 0)
            {
                target = nullptr;
            }
        }
        float xWalkDist = posX - walkToX;
        float yWalkDist = posY - walkToY;

        if (walkToX != posX && walkToY == posY)
        {
            velocityX = -xWalkDist / std::abs(xWalkDist) * _class.runSpeed * delta_time;
        }
        if (walkToY != posY && walkToX == posX)
        {
            velocityY = -yWalkDist / std::abs(yWalkDist) * _class.runSpeed * delta_time;
        }
        if (walkToX != posX && walkToY != posY)
        {
            float normalizationCap = std::max(std::abs(xWalkDist), std::abs(yWalkDist));
            velocityX = -xWalkDist / normalizationCap * _class.runSpeed * delta_time;
            velocityY = -yWalkDist / normalizationCap * _class.runSpeed * delta_time;
        }
        // if (std::abs(xWalkDist) < _class.runSpeed * delta_time * 1.2f) // find a good way to eliminate the overshoot (jittering)
        //     posX = walkToX;
        // if (std::abs(yWalkDist) < _class.runSpeed * delta_time * 1.2f)
        //     posY = walkToY;

        if (animationFinished)
        {
            PlayAnimation(ANIM_IDLE, delta_time, true);
        }

        animations[playingAnim].run(delta_time, animationLooping);

        visual->Put(posX + screenOffsetX, posY + screenOffsetY);

        animationFinished = false;

        if (animations[playingAnim].finished)
            animationFinished = true;
    }
    void updatePosition()
    {
        posX += velocityX;
        posY += velocityY;
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
    int firstUnitSelected = -1;

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
                if (allies[i].visual != nullptr)
                    allies[i].visual->rect.setColor(sf::Color(255, 255, 255, 170));
            }
            else
            {
                selected[i] = false;
                if (allies[i].visual != nullptr)
                    allies[i].visual->rect.setColor(sf::Color(255, 255, 255, 255));
            }
        }
    }
};

int qsPartition(sprite *sprites[entity_limit], int low, int high) // see if there's a better optimized way to do this, such as random pivot
{
    sprite *pivot = sprites[low];

    int i = high + 1;
    for (int j = low + 1; j <= high; ++j)
    {
        if (sprites[j]->rect.getPosition().y > pivot->rect.getPosition().y)
        {
            --i;
            std::swap(sprites[i], sprites[j]);
        }
    }

    std::swap(sprites[i - 1], sprites[low]);
    return i - 1;
}
void quicksortSprites(sprite *sprites[entity_limit], int low, int high)
{
    if (low < high)
    {
        int pi = qsPartition(sprites, low, high);

        quicksortSprites(sprites, low, pi - 1);
        quicksortSprites(sprites, pi + 1, high);
    }
}

// quadtree and collision stuff probably should go in game_system
struct game_system
{
    character *characters[entity_limit];
    sprite *sortedSprites[entity_limit];
    int characterCount = 0;
    bool paused = false;
    terrain ground, air;

    void Add(player *p)
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (p->allies[i].visual == nullptr)
                continue;

            characters[characterCount] = &p->allies[i];
            sortedSprites[characterCount] = characters[characterCount]->visual;
            ++characterCount;
        }
    }
    void Add(character *e)
    {
        characters[characterCount] = e;
        sortedSprites[characterCount] = characters[characterCount]->visual;
        ++characterCount;
    }

    void update(dungeon &floor, float delta_time)
    {
        if (paused)
        {
            for (int i = 0; i < characterCount; ++i)
            {
                characters[i]->visual->Put(characters[i]->posX + floor.screenPositionX, characters[i]->posY + floor.screenPositionY);
            }
            return;
        }

        quicksortSprites(sortedSprites, 0, characterCount - 1);
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
            characters[i]->Update(delta_time, floor.screenPositionX, floor.screenPositionY);

            for (int j = 0; j < floor.collision_box_count; ++j)
            {
                if (floor.collision_boxes[j].min_x == 0.0f &&
                    floor.collision_boxes[j].min_y == 0.0f &&
                    floor.collision_boxes[j].max_x == 0.0f &&
                    floor.collision_boxes[j].max_y == 0.0f)
                {
                    continue;
                }

                float playerMoveSpeedX = characters[i]->_class.runSpeed;
                if (characters[i]->walkToX < characters[i]->posX)
                    playerMoveSpeedX = -characters[i]->_class.runSpeed;

                float playerMoveSpeedY = characters[i]->_class.runSpeed;
                if (characters[i]->walkToY < characters[i]->posY)
                    playerMoveSpeedY = -characters[i]->_class.runSpeed;

                if (characters[i]->walkToX == characters[i]->posX)
                    playerMoveSpeedX = 0.0f;
                if (characters[i]->walkToY == characters[i]->posY)
                    playerMoveSpeedY = 0.0f;

                float xNormal = 0.0f;

                float firstCollisionHitTest = characters[i]->collider.response(playerMoveSpeedX * delta_time,
                                                                               playerMoveSpeedY * delta_time, floor.collision_boxes[j], xNormal);

                if (firstCollisionHitTest < 1.0f && xNormal == 1.0f)
                {
                    characters[i]->velocityX *= firstCollisionHitTest;
                }
                if (firstCollisionHitTest < 1.0f && xNormal == 0.0f)
                {
                    characters[i]->velocityY *= firstCollisionHitTest;
                }
            } // quadtree implementation is next
            characters[i]->updatePosition();
        }
    }
};

void ability_simpleMelee(int value, attack *thisAttack, character *parent)
{
    for (int i = 0; i < thisAttack->targetCount; ++i)
    {
        if (thisAttack->targets[i] == nullptr)
            continue;

        float distanceX = parent->posX - thisAttack->targets[i]->posX;
        float distanceY = parent->posY - thisAttack->targets[i]->posY;

        float dist = distanceX * distanceX + distanceY * distanceY;
        if (dist > thisAttack->distanceRequired)
            continue;

        thisAttack->targets[i]->hp -= value;
    }
}
void ability_brawler_hook(int value, attack *thisAttack, character *parent)
{
    static int attackCount = 0;
    for (int i = 0; i < thisAttack->targetCount; ++i)
    {
        if (thisAttack->targets[i] == nullptr)
            continue;

        float distanceX = parent->posX - thisAttack->targets[i]->posX;
        float distanceY = parent->posY - thisAttack->targets[i]->posY;

        float dist = distanceX * distanceX + distanceY * distanceY;
        if (dist > thisAttack->distanceRequired)
            continue;

        ++attackCount;

        thisAttack->targets[i]->hp -= value;
        if (attackCount > 2)
        {
            attackCount = 0;
            ++parent->nextAbility;
        }
        parent->attackTimer = parent->_class.attackSpeed * 0.2f;
    }
}
void ability_brawler_right_hook(int value, attack *thisAttack, character *parent)
{
    for (int i = 0; i < thisAttack->targetCount; ++i)
    {
        if (thisAttack->targets[i] == nullptr)
            continue;

        float distanceX = parent->posX - thisAttack->targets[i]->posX;
        float distanceY = parent->posY - thisAttack->targets[i]->posY;

        float dist = distanceX * distanceX + distanceY * distanceY;
        if (dist > thisAttack->distanceRequired)
            continue;

        thisAttack->targets[i]->hp -= value * 1.5f;
        --parent->nextAbility;
        parent->attackTimer = parent->_class.attackSpeed * 1.6f;
    }
}

#endif
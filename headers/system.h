#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>

const unsigned int att_limit = 4;
const unsigned int stat_limit = 4;
const unsigned int entity_limit = 64;

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

    unsigned int initiative = 0;
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

#endif
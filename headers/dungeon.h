#ifndef DUNGEON_H
#define DUNGEON_H

#include <random>

struct room
{
    unsigned int width = 4, height = 4;
    bool start = true, end = false;

    room()
    {
        // random width and height
    }
};

#endif
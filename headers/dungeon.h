#ifndef DUNGEON_H
#define DUNGEON_H

#include <random>
#include "sprite.h"

const unsigned int room_limit = 12;
const unsigned int width = 4;
const unsigned int height = 4;

struct room
{
    bool start = true, end = false;
    sprite tiles[width][height];

    room(const char *wallImgPath, const char *floorImgPath, float scaleX, float scaleY)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            for (unsigned int y = 0; y < height; ++y)
            {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
                {
                    tiles[x][y] = sprite(wallImgPath, static_cast<float>(x), static_cast<float>(y), scaleX * 16.0f, scaleY * 16.0f, 1, 1);
                }
                else
                {
                    tiles[x][y] = sprite(floorImgPath, static_cast<float>(x), static_cast<float>(y), scaleX * 16.0f, scaleY * 16.0f, 1, 1);
                }

                tiles[x][y].Put(static_cast<float>(x) * tiles[x][y].spriteW * scaleX, static_cast<float>(y) * tiles[x][y].spriteH * scaleY);
            }
        }

        // random width and height
    }

    void draw(sf::RenderWindow *win)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            for (unsigned int y = 0; y < height; ++y)
            {
                win->draw(tiles[x][y].rect);
            }
        }
    }
};

struct dungeon
{
    room rooms[room_limit];
};

#endif
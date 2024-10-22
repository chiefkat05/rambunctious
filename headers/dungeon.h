#ifndef DUNGEON_H
#define DUNGEON_H

#include <random>
#include <fstream>
#include "sprite.h"
#include "collision.h"

const unsigned int room_limit = 12;
const unsigned int width_limit = 64;
const unsigned int height_limit = 64;
const unsigned int collision_box_limit = 64;
const unsigned int entity_limit_2 = 64;

#define NULL_TILE -2147483647

struct dungeon
{
    bool start = true, end = false;
    sprite tiles[width_limit][height_limit];

    sprite enemies[entity_limit_2];
    int enemyCount = 0;

    unsigned int roomWidth = 0, roomHeight = 0;
    unsigned int tileSpriteX, tileSpriteY;
    float spawnLocationX = 0.0f, spawnLocationY = 0.0f;

    aabb collision_boxes[collision_box_limit];
    unsigned int collision_box_count = 0;

    float screenPositionX = 0.0f, screenPositionY = 0.0f, lastScreenPosX = 0.0f, lastScreenPosY = 0.0f, screenChangeDistanceX, screenChangeDistanceY;

    dungeon(const char *tileSetPath, const unsigned int xSize, const unsigned int ySize, float massScale, float massYOffset)
    {
        for (unsigned int x = 0; x < width_limit; ++x)
        {
            for (unsigned int y = 0; y < height_limit; ++y)
            {
                tiles[x][y] = sprite(tileSetPath, static_cast<float>(x), static_cast<float>(y), xSize * massScale, ySize * massScale, 1, 1);
                tiles[x][y].Put(static_cast<float>(x) * tiles[x][y].spriteW, static_cast<float>(y) * tiles[x][y].spriteH);
            }
        } // the pointer to the ui_element's visual object is incorrect inside the element's update function
    } // also do some lmms pls

    // void generateWallCollisions()
    // {

    // }

    void updateScreenPosition(float mouseX, float mouseY, float delta_time, float massScale, float massYOffset)
    { // add a scrollspeed variable for easy, also remember attack selection is offput by screenscroll, which is incorrect
        if (mouseX < 10.0f / massScale)
            screenPositionX += (80.0f / massScale - (mouseX * 8.0f)) * delta_time * massScale;
        if (mouseX > 246.0f / massScale)
            screenPositionX -= ((mouseX - 246.0f / massScale) * 8.0f) * delta_time * massScale;
        if (mouseY < 10.0f / massScale)
            screenPositionY += (80.0f / massScale - (mouseY * 8.0f)) * delta_time * massScale;
        if (mouseY > 118.0f / massScale + (massYOffset * 2.0f / massScale))
            screenPositionY -= ((mouseY - (118.0f / massScale + (massYOffset * 2.0f / massScale))) * 8.0f) * delta_time * massScale;

        float screenXPanLimit = 10.0f + (static_cast<float>(std::max(0, static_cast<int>(roomWidth) - 14)) * 16.0f);
        if (screenPositionX < -screenXPanLimit) // something's weird with the .sdf creation
        {                                       // I hate it
            screenPositionX = -screenXPanLimit;
        }
        if (screenPositionX > 10.0f)
        {
            screenPositionX = 10.0f;
        }
        float screenYPanLimit = 10.0f + (static_cast<float>(std::max(0, static_cast<int>(roomHeight) - 7)) * 16.0f);
        if (screenPositionY < -screenYPanLimit)
        {
            screenPositionY = -screenYPanLimit;
        }
        if (screenPositionY > 10.0f)
        {
            screenPositionY = 10.0f;
        }
    }

    void draw(sf::RenderWindow *win)
    {
        for (unsigned int x = 0; x < width_limit; ++x)
        {
            for (unsigned int y = 0; y < height_limit; ++y)
            {
                if (tiles[x][y].rect.getPosition().x == NULL_TILE && tiles[x][y].rect.getPosition().y == NULL_TILE)
                    continue;
                if (lastScreenPosX != screenPositionX || lastScreenPosY != screenPositionY)
                {
                    tiles[x][y].Put(static_cast<float>(x) * 16.0f + screenPositionX,
                                    static_cast<float>(y) * 16.0f + screenPositionY);
                }

                win->draw(tiles[x][y].rect);
            }
        }

        screenChangeDistanceX = lastScreenPosX - screenPositionX;
        screenChangeDistanceY = lastScreenPosY - screenPositionY;

        lastScreenPosX = screenPositionX;
        lastScreenPosY = screenPositionY;
    }

    void readRoomFile(const char *path, float mScale, float mYOffset)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cout << "Failed to open " << path << "\n";
            return;
        }

        std::string line;
        roomHeight = 0;

        for (int i = 0; i < width_limit; ++i)
        {
            for (int j = 0; j < height_limit; ++j)
            {
                tiles[i][j].Put(NULL_TILE, NULL_TILE);
            }
        }

        while (std::getline(file, line))
        {
            if (line.size() > roomWidth)
                roomWidth = line.size();

            unsigned int widthLim = std::min(static_cast<unsigned int>(line.size()), width_limit);
            for (unsigned int i = 0; i < widthLim; ++i)
            {
                tiles[i][roomHeight].Put(static_cast<float>(i) * 16.0f,
                                         static_cast<float>(roomHeight) * 16.0f);

                switch (line[i])
                {
                case '0':
                    tiles[i][roomHeight].rect.setTextureRect(sf::IntRect(0, 0, 16, 16));
                    break;
                case '1':
                    tiles[i][roomHeight].rect.setTextureRect(sf::IntRect(16, 0, 16, 16));
                    collision_boxes[collision_box_count] = aabb(tiles[i][roomHeight].rect.getPosition().x, tiles[i][roomHeight].rect.getPosition().y,
                                                                tiles[i][roomHeight].rect.getPosition().x + 16.0f,
                                                                tiles[i][roomHeight].rect.getPosition().y + 16.0f);
                    ++collision_box_count;
                    break;
                case 's':
                    tiles[i][roomHeight].rect.setTextureRect(sf::IntRect(0, 0, 16, 16));
                    spawnLocationX = tiles[i][roomHeight].rect.getPosition().x;
                    spawnLocationY = tiles[i][roomHeight].rect.getPosition().y;
                    break;
                case 'e':
                    tiles[i][roomHeight].rect.setTextureRect(sf::IntRect(0, 0, 16, 16));
                    enemies[enemyCount] = sprite("../img/enemies/dungeon-1/megdrer.png", tiles[i][roomHeight].rect.getPosition().x,
                                                 tiles[i][roomHeight].rect.getPosition().y, 16.0f, 16.0f, 1, 1);

                    ++enemyCount;
                default:
                    tiles[i][roomHeight].rect.setTextureRect(sf::IntRect(0, 0, 16, 16));
                    break;
                }
            }
            ++roomHeight;
        }
    }
};

#endif
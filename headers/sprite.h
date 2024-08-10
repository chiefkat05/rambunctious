#ifndef SPRITE_H
#define SPRITE_H

#include "setup.h"

const unsigned int texture_limit = 24;

struct texturePile
{
    sf::Texture list[texture_limit];
    const char *paths[texture_limit];

    unsigned int count = 0;
};

struct sprite
{
    float w, h, walkToX, walkToY;
    unsigned int spriteW, spriteH;
    unsigned int framesX, framesY;
    std::string path;
    sf::Sprite rect;
    unsigned int img;

    sprite();

    sprite(const char *_path, float _x, float _y, float _w, float _h);

    void Put(float _x, float _y);
    void Move(float _xDist, float _yDist);
};

struct animation
{
    unsigned int start = 0, end = 0;

    float speed = 1.0f;
    float timer = 10.0f;
    unsigned int frame = 0;

    sprite *_sprite;

    animation(sprite *sp, unsigned int s, unsigned int e = 1, float spd = 1.0f) : _sprite(sp)
    {
        start = s;
        end = e;
        speed = spd;
    }

    void run(float delta_time)
    {
        if (timer <= 0.0f)
        {
            timer = 10.0f;

            frame >= end ? frame = 0 : ++frame;
        }

        timer -= speed * delta_time;

        _sprite->rect.setTextureRect(sf::IntRect(frame % _sprite->framesX * 16.0f, frame / _sprite->framesX * 16.0f, 16, 16));
    }
};

#endif
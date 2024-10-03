#include "../headers/sprite.h"

texturePile tPile;

sprite::sprite()
{
    w = 0.0f;
    h = 0.0f;
    walkToX = 0.0f;
    walkToY = 0.0f;
    spriteW = 0;
    spriteH = 0;
    framesX = 0;
    framesY = 0;
    path = "";
    img = 0;
}

sprite::sprite(const char *_path, float _x, float _y, float _w, float _h, unsigned int _fx, unsigned int _fy)
{
    bool foundMatchingTexture = false;
    for (unsigned int i = 0; i < tPile.count; ++i)
    {
        if (tPile.paths[i] == _path)
        {
            img = i;
            foundMatchingTexture = true;
            break;
        }
    }

    if (!foundMatchingTexture && !tPile.list[tPile.count].loadFromFile(_path))
    {
        std::cout << "error: image " << _path << " failed to load.\n";
        return;
    }
    tPile.list[tPile.count].setRepeated(true);

    if (!foundMatchingTexture)
    {
        tPile.paths[tPile.count] = _path;
        img = tPile.count;

        ++tPile.count;
    }

    rect = sf::Sprite(tPile.list[img]);
    framesX = _fx;
    framesY = _fy;

    path = _path;

    rect.setPosition(sf::Vector2(_x, _y));

    spriteW = tPile.list[img].getSize().x / framesX;
    spriteH = tPile.list[img].getSize().y / framesY;

    rect.setScale(sf::Vector2(_w / spriteW, _h / spriteH));
}

void sprite::Put(float _x, float _y)
{
    rect.setPosition(sf::Vector2(_x, _y));
}
void sprite::Move(float _xDist, float _yDist)
{
    rect.move(sf::Vector2(_xDist, _yDist));
}

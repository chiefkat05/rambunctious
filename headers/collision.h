#ifndef COLLISION_H
#define COLLISION_H

#include <iostream>

struct aabb
{
    float min_x, min_y, max_x, max_y;

    aabb() : min_x(0.0f), min_y(0.0f), max_x(0.0f), max_y(0.0f) {}
    aabb(float x, float y, float x2, float y2) : min_x(x), min_y(y), max_x(x2), max_y(y2) {}

    bool colliding(aabb &test)
    {
        if (min_x > test.max_x || min_y > test.max_y ||
            test.min_x > max_x || test.min_y > max_y)
            return false;

        return true;
    }

    void moveCenterToPoint(float xPos, float yPos)
    {
        float width = max_x - min_x;
        float height = max_y - min_y;
        min_x = xPos - width * 0.5f;
        min_y = yPos - height * 0.5f;
        max_x = xPos + width * 0.5f;
        max_y = yPos + height * 0.5f;
    }

    float response(float xVelocity, float yVelocity, aabb &test, float &xNormal)
    {
        aabb boundingbox = aabb(min_x + std::min(xVelocity, 0.0f), min_y + std::min(yVelocity, 0.0f),
                                max_x + std::max(xVelocity, 0.0f), max_y + std::max(yVelocity, 0.0f));
        if (!test.colliding(boundingbox))
            return 1.0f;

        float tFirst = 0.0f;
        float tLast = 1.0f;
        float xEntry = 0.0f, yEntry = 0.0f;

        if (xVelocity < 0.0f)
            xEntry = (min_x - test.max_x) / xVelocity;
        if (xVelocity > 0.0f)
            xEntry = (test.min_x - max_x) / xVelocity;
        if (yVelocity < 0.0f)
            yEntry = (min_y - test.max_y) / yVelocity;
        if (yVelocity > 0.0f)
            yEntry = (test.min_y - max_y) / yVelocity; // Add a camera lock-on to units at game start! Concept some more! Plan next move! Don't waste time because you can't figure out what to do next! Put on some smooth jazz!!!

        if (std::abs(xEntry) < std::abs(yEntry))
            xNormal = 1.0f;
        else
            xNormal = 0.0f;

        if (colliding(test))
        {
            return 0.0f;
        }

        if (xVelocity < 0.0f)
        {
            if (test.max_x < min_x)
                return 0.0f;
            if (max_x < test.min_x)
                tFirst = std::max((max_x - test.min_x) / xVelocity, tFirst);
            if (test.max_x > min_x)
                tLast = std::min((min_x - test.max_x) / xVelocity, tLast);
        }
        if (xVelocity > 0.0f)
        {
            if (test.min_x > max_x)
                return 0.0f;
            if (test.max_x < min_x)
                tFirst = std::max((min_x - test.max_x) / xVelocity, tFirst);
            if (max_x > test.min_x)
                tLast = std::min((max_x - test.min_x) / xVelocity, tLast);
        }
        if (yVelocity < 0.0f)
        {
            if (test.max_y < min_y)
                return 0.0f;
            if (max_y < test.min_y)
                tFirst = std::max((max_y - test.min_y) / yVelocity, tFirst);
            if (test.max_y > min_y)
                tLast = std::min((min_y - test.max_y) / yVelocity, tLast);
        }
        if (yVelocity > 0.0f)
        {
            if (test.min_y > max_y)
                return 0.0f;
            if (test.max_y < min_y)
                tFirst = std::max((min_y - test.max_y) / yVelocity, tFirst);
            if (max_y > test.min_y)
                tLast = std::min((max_y - test.min_y) / yVelocity, tLast);
        }

        if (tFirst > tLast)
            return 0.0f;

        return tFirst;
    }
};

const unsigned int quadtree_object_limit = 5;
struct quadtree
{
    aabb bounds;
    quadtree *bottomleft, *bottomright, *topleft, *topright;

    aabb objects[quadtree_object_limit];
};

#endif
#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include "setup.h"

const unsigned int sound_limit = 16;

struct soundhandler
{
    sf::SoundBuffer buffer;
    sf::Sound sound;
    int preparedSound = 0;

    void load(const std::string filepath)
    {
        if (!buffer.loadFromFile(filepath))
        {
            std::cout << "! sound effect at " << filepath << " failed to load.\n";
            return;
        }
    }
    void play()
    {
        sound.setBuffer(buffer);
        sound.play();
    }
};

#endif
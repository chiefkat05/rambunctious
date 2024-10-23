#ifndef EFFECTS_H
#define EFFECTS_H

#include "setup.h"
#include "sprite.h"
#include <random>

const unsigned int sound_limit = 16;
const unsigned int particle_limit = 57;
const unsigned int particle_system_limit = 6;

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

std::default_random_engine numGen;

struct particle
{
    sprite visual;
    float velX, velY, life;
    float lifestartalphamultiple = 1.0f;
};

struct particlesystem
{
    particle particles[particle_limit];
    unsigned int particle_count, particles_alive;
    bool fadewithlife = true;

    particlesystem(const char *path, float width, float height, unsigned int textureX, unsigned int textureY, unsigned int textureW, unsigned int textureH, unsigned int _pc)
    {
        particle_count = particle_limit;
        if (_pc < particle_limit)
            particle_count = _pc;

        for (int i = 0; i < particle_count; ++i)
        {
            particles[i].visual = sprite(path, 0.0f, 0.0f, width, height, 1, 1);
            particles[i].visual.rect.setTextureRect(sf::IntRect(textureX, textureY, textureW, textureH));
        }
    }

    void spawn(float x, float y, float w, float h, float lifeLower, float lifeUpper, float pushXmin, float pushYmin, float pushXmax, float pushYmax, float screenOffsetX, float screenOffsetY)
    {
        if (particles_alive >= particle_count)
            return;

        std::uniform_real_distribution<double> posXRand(x, w);
        std::uniform_real_distribution<double> posYRand(y, h);
        std::uniform_real_distribution<double> lifeRand(lifeLower, lifeUpper);

        particles[particles_alive].visual.Put(posXRand(numGen) + screenOffsetX, posYRand(numGen) - screenOffsetY);
        particles[particles_alive].velX = 0.0f;
        particles[particles_alive].velY = 0.0f;
        particles[particles_alive].life = lifeRand(numGen);
        particles[particles_alive].lifestartalphamultiple = 255.0f / particles[particles_alive].life;
        push(particles_alive, pushXmin, pushYmin, pushXmax, pushYmax);
        ++particles_alive;

        // for (int i = 0; i < particle_limit; ++i)
        // {
        //     particles[i].visual.Put(posXRand(numGen), posYRand(numGen));
        //     particles[i].life = lifeRand(numGen);
        // }
    }
    void push(unsigned int index, float xVel, float yVel, float xVelMax, float yVelMax)
    {
        std::uniform_real_distribution<double> parXVel(xVel, xVelMax);
        std::uniform_real_distribution<double> parYVel(yVel, yVelMax);

        particles[index].velX = parXVel(numGen);
        particles[index].velY = parYVel(numGen);
    }

    void update(float delta_time, float screenChangeX, float screenChangeY)
    {
        for (int i = 0; i < particles_alive; ++i)
        {
            if (particles[i].life < 0.0f)
            {
                particles[i] = particles[particles_alive - 1];
                --particles_alive;
                continue;
            }

            particles[i].visual.Move(particles[i].velX * delta_time - screenChangeX, particles[i].velY * delta_time - screenChangeY);
            particles[i].life -= 10.0f * delta_time;
            if (fadewithlife)
                particles[i].visual.rect.setColor(sf::Color(255, 255, 255, static_cast<int>(particles[i].life * particles[i].lifestartalphamultiple)));
        }
    }
};

// struct particlehandler
// {
//     // play, setsprite, color, stop
//     particlesystem systems[particle_system_limit];
// };

#endif
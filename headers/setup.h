#ifndef SETUP_H
#define SETUP_H

#include <SFML/Graphics.hpp>

void init()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!");

    sf::CircleShape circle(100.0f);
    circle.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(circle);
        window.display();
    }
}

#endif
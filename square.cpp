#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include <thread>
#include <chrono>

#define W_X 1000
#define W_Y 1000

void drawLine(sf::Vector2f st, sf::Vector2f en, sf::RenderWindow& wind)
{
    sf::VertexArray lines(sf::Lines, 2);
    lines[0].position = st;
    lines[1].position = en;
    lines[0].color = lines[1].color = sf::Color::Red;
    wind.draw(lines);
}

inline sf::Vector2f normalize(sf::Vector2f point)
{
    return {
        W_X/2 + point.x,
        W_Y/2 - point.y
    };
}

constexpr float _an = 90*M_PI/180;

void drawSquare(float size, float angle, sf::RenderWindow& window)
{
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    float rad = size/2;

    // printf("cos: %f\nsin: %f\n", cosAngle, sinAngle);

    /*
        l1
    *----------*
    |          |
    |          |
  l4|          | l2
    |          |
    |          |
    *----------*
         l3
    */
    drawLine(
        normalize({ -rad*cosAngle - rad*sinAngle, -rad*sinAngle + rad * cosAngle}),
        normalize({ rad*cosAngle - rad*sinAngle, rad*sinAngle + rad * cosAngle}),
        window
    ); // l1
    drawLine(
        normalize({ -rad*cosAngle + rad*sinAngle, -rad*sinAngle - rad * cosAngle}),
        normalize({ rad*cosAngle + rad*sinAngle, rad*sinAngle - rad * cosAngle}),
        window
    ); //l3
    drawLine(
        normalize({ -rad*cosAngle - rad*sinAngle, -rad*sinAngle +rad*cosAngle}),
        normalize({ -rad*cosAngle + rad*sinAngle, -rad*sinAngle -rad*cosAngle}),
        window
    ); // l4
    drawLine(
        normalize({ rad*cosAngle - rad*sinAngle, rad*sinAngle +rad*cosAngle}),
        normalize({ rad*cosAngle + rad*sinAngle, rad*sinAngle -rad*cosAngle}),
        window
    ); // l2
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(W_X, W_Y), "MainWindow");
    sf::Event event;
    sf::Clock timer;
    float angle = 0;
    float size = 1;
    window.clear();

    while(window.isOpen())
    {
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                return 1; // всё.
            }
        }
        if(size > W_X*1.2) goto nextframe;
        // window.clear();
        drawSquare(size, angle, window);
        angle += 2*M_PI/180;
        size += 1;
        window.display();
nextframe:
        std::this_thread::sleep_for(std::chrono::milliseconds(10 - timer.getElapsedTime().asMilliseconds()));
        timer.restart();
    }
    return 0;
}

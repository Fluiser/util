#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <time.h>
#include <stdlib.h>
#include <iostream>

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

constexpr float _an = 2*M_PI/180;

void drawSquare(int size, float angle, sf::RenderWindow& window)
{
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    float rad = size/2.0f;

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

struct square {
    float angle;
    int op = 1;
    int size;
    inline static const int maxSize = 800;
};

int main()
{
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(W_X, W_Y), "MainWindow");
    sf::Event event;
    std::vector<square> sqs{};

    int count = 0;
    printf("count: ");
    std::cin >> count;
    printf("try reesize\n");
    sqs.resize(count);
    printf("sqs: %i\n", sqs.size());

    for(auto& sq: sqs)
    {
        sq.angle = (rand()%360) * _an;
        sq.size = rand()%square::maxSize;
    }
    window.setActive(false);

    std::thread renderThread([&]{
        window.setActive(true);
        sf::Clock timer;

        while(window.isOpen())
        {
            window.clear();
            for(auto& sq: sqs)
            {
                drawSquare(sq.size, sq.angle, window);
                sq.angle += _an*sq.op;
                if(abs(sq.size) >= square::maxSize)
                {
                    sq.op *= -1;
                    sq.size += sq.op;
                }
                else
                {
                    sq.size += sq.op;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30-timer.getElapsedTime().asMilliseconds()));
            timer.restart();
            window.display();
        }
    });

    while(window.isOpen())
    {
        while(window.waitEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                return 1; // всё.
            }
        }
    }
    return 0;
}

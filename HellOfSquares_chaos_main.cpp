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
#include <mutex>
#include <Windows.h>
#include <string>

int W_X = 1920;
int W_Y = 1080;
decltype(sf::Style::Default) styleDefWindow = sf::Style::Default;

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
    float op = 1;
    int size;
    inline static int maxSize = 0;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int count = 1;
    {
        for(size_t i = 0; pCmdLine[i] != '\0'; ++i)
        {
            if(pCmdLine[i] != L' ')
            {
                int j = i;
                for(; pCmdLine[j] != L' ' && pCmdLine[j] != '\0'; ++j);
                std::wstring field(pCmdLine + i, pCmdLine + j);
                ++j;
                i=j;
                if(field == L"count")
                {
                    for(; pCmdLine[j] != L' ' && pCmdLine[j] != '\0'; ++j);
                    std::wstring val(pCmdLine + i, pCmdLine + j);
                    count = std::stoi(val);
                }
                if(field == L"x")
                {
                    for(; pCmdLine[j] != L' ' && pCmdLine[j] != '\0' ; ++j);
                    std::wstring val(pCmdLine + i, pCmdLine + j);
                    W_X = std::stoi(val);
                }
                if(field == L"y")
                {
                    for(; pCmdLine[j] != L' ' && pCmdLine[j] != '\0'; ++j);
                    std::wstring val(pCmdLine + i, pCmdLine + j);
                    W_Y = std::stoi(val);
                }
                if(field == L"full")
                {
                    styleDefWindow = sf::Style::Fullscreen;
                }
                i = j;
            }
        }
    }

    square::maxSize = sqrt(W_X * W_Y);

    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(W_X, W_Y), "MainWindow", styleDefWindow);
    sf::Event event;
    std::vector<square> sqs{};

    sqs.resize(count);

    std::mutex mtx;

    for(auto& sq: sqs)
    {
        sq.angle = (rand()%360) * _an*1;
        sq.size = rand()%square::maxSize;
        sq.op = ((rand()%5000)/5000 + 0.6) * (rand() >= RAND_MAX/2 ? 1 : -1);
    }
    window.setActive(false);

    bool activeByEventThread = true;

    std::thread renderThread([&]{
        sf::Clock timer;

        while(window.isOpen())
        {
            mtx.lock();
            if(activeByEventThread)
            {
                if(activeByEventThread)
                {
                    if(activeByEventThread)
                    {
                        window.setActive();
                        if(activeByEventThread)
                            activeByEventThread = false;
                    }
                }
            }
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
            window.display();
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(30-timer.getElapsedTime().asMilliseconds()));
            timer.restart();
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
            if(event.type == sf::Event::KeyPressed)
            {
                std::lock_guard<std::mutex> __(mtx);
                printf("ok");
            	if(sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
                {
                    window.setActive();
                    window.close();
                    window.create(sf::VideoMode(W_X, W_Y), "FullscreenWindow", sf::Style::Fullscreen);
                    window.setActive(false);
                    activeByEventThread = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::F10))
                {
                    window.setActive();
                    window.close();
                    window.create(sf::VideoMode(W_X, W_Y), "DefaultWindow", sf::Style::Default);
                    window.setActive(false);
                    activeByEventThread = true;
                }
            }
        }
    }
    return 0;
}
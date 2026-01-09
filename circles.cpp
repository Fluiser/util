#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <chrono>
#include <cstdlib>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <iostream> 

float hue2rgb(float p, float q, float t) {
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;
	if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f) return q;
	if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
	return p;
}

uint32_t hslToRgb(float h, float s, float l) {
	float r, g, b;
	float q = l < 0.5 ? l * (1.0f + s) : l + s - l * s;
	float p = 2.0f * l - q;
	r = hue2rgb(p, q, h + 1.0f / 3.0f);
	g = hue2rgb(p, q, h);
	b = hue2rgb(p, q, h - 1.0f / 3.0f);
	if (r < 0.1 && g < 0.1 && b < 0.1)
		std::cout << h << " " << s << " " << l << "\n";
	return ((int)round(r * 255.0f) << 24 | ((int)round(g * 255.0f) << 16) | ((int)round(b * 255.0f) << 8));
}

void line(sf::RenderWindow& w, sf::Vector2f str, sf::Vector2f end, sf::Color color)
{
    sf::VertexArray arr(sf::Lines, 2);
    arr[0].color = arr[1].color = color;
    arr[0].position = str;
    arr[1].position = end;

    w.draw(arr);
}

void drawCircle(sf::RenderWindow& window, sf::Vector2f pos, float rad, float startAngle, float endAngle, sf::Color color)
{
    if(endAngle < startAngle) {
        float temp = startAngle;
        startAngle = endAngle; 
        endAngle = temp; 
    }

    constexpr static float step = M_PI/90;
    for(float x = startAngle; x < endAngle; x += step)
    {
        line(window, 
                sf::Vector2f { cos(x) * rad, sin(x) * rad  } + pos,
                sf::Vector2f { cos(x + step) * rad, sin(x + step) * rad  } + pos
                    ,color 
                );
    }
}

struct circle {
    float rad; 
    float angle_s;
    float angle;
    float speed;
    char indexColor = 0;
};

#define COLOR_SIZE 100
sf::Color colors[COLOR_SIZE];

int main(int argc, char** argv)
{
    int count = argc > 1 ? std::stoi(argv[1]) : 300;
    srand(time(0));

    sf::ContextSettings ctx;
    ctx.antialiasingLevel = 64;

    sf::RenderWindow window(sf::VideoMode(1280, 720), "mainwindow", sf::Style::Default, ctx);
    sf::Event event;

    for(int i = 1; i <= COLOR_SIZE; ++i)
    {
        int f = hslToRgb((float)i / COLOR_SIZE, 1.0f, 0.5f);
        colors[i-1] = sf::Color(f | 0xff);
    }
    
    sf::Vector2f center = {640, 360};

    std::vector<circle> circles; 

    for(int i = 0; i < count; ++i)
    {
        float as = M_PI*2.0f*((float)rand() / (float)RAND_MAX);
        float ae = M_PI*0.350f*((float)rand()/(float)RAND_MAX);

        circles.emplace_back(circle{
                    .rad = 1.0f + static_cast<float>(rand() % 601), 
                    .angle_s = as,
                    .angle = ae,
                    .speed = (float)M_PI * 1.50f * ((float)rand()/(float)RAND_MAX),    
                    .indexColor = (char)(rand() % COLOR_SIZE)
            });
    }

    std::thread upader([&circles]{
        sf::Clock timer;

        while(1)
        {
            timer.restart();

            for(auto& x: circles)
            {
                x.angle_s += (x.speed/x.rad);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(30 - timer.getElapsedTime().asMilliseconds()));
        }
    });

    sf::Clock timer;

    while(window.isOpen())
    {
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed) return 0;
        }
        window.clear();
        timer.restart();
        
        for(const auto& x: circles)
        {
            drawCircle(window, center, x.rad, x.angle_s, x.angle_s + x.angle, colors[x.indexColor]);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30 - timer.getElapsedTime().asMilliseconds()));
        window.display();
    
    }

    return 0;
}

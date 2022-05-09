#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>


#define W_X 1920
#define W_Y 1920
#define MAX_ITERATION 50
#define THREAD_SIZE 4

static double scale = 0.30;
sf::Vector2<double> offset{};

void drawLine(sf::Vector2f st, sf::Vector2f en, sf::RenderWindow& wind)
{
    sf::VertexArray lines(sf::Lines, 2);
    lines[0].position = st;
    lines[1].position = en;
    lines[0].color = lines[1].color = sf::Color(0xff, 0xff, 0xff, 13);
    wind.draw(lines);
}

double ___get_x(int x)
{
    return ((double)x / W_X - 0.5) / scale - offset.x;
}
double ___get_y(int y)
{
    return ((double)y / W_Y - 0.5) / scale - offset.y;
}
void square_complex(double* mx)
{
    double re = mx[0] * mx[0] - mx[1] * mx[1];
    double im = mx[0] * mx[1] +  mx[1] * mx[0];
    mx[0] = re;
    mx[1] = im;
}
void cos_complex(double* mx)
{
    double re = mx[0];
    double im = mx[1];
    mx[0] = cos(re) * cosh(im);
    mx[1] = sin(re) * sinh(im) * -1;
}
void sin_complex(double* mx)
{
    double re = mx[0];
    double im = mx[1];
    mx[0] = sin(re) * cosh(im);
    mx[1] = cos(re) * sinh(im);
}
void plus_copmex(double* src, const double* l)
{
    src[0] = src[0] + l[0];
    src[1] = src[1] + l[1];
}
void divide_complex(double* src, const double* left, const double* right)
{
    double div = pow(left[0],2) + pow(left[1], 2);
    src[0] = (left[0] * right[0] + left[1] * right[1] / div);
    src[1] = (left[1] * right[0] - left[0] * right[1] / div);
}
void exp_complex(double* mx)
{
    mx[0] = exp(mx[0]);
}
int main()
{
    sf::RenderWindow window(sf::VideoMode(W_X, W_Y), "M", sf::Style::Fullscreen);

    sf::Event event;
    sf::Image img;
    img.create(W_X, W_Y, sf::Color::White);

    double b = 0.3891;
    double alpha = 0;
    double* c0 = new double[2];
    c0[0] = b * sin(alpha);
    c0[1] = b * cos(alpha);

    // drawTexture(img, c0);
    sf::Texture texture;
    sf::Sprite sprite;

    texture.loadFromImage(img);
    sprite.setTexture(texture);

    window.setActive(false);

    std::thread* threads[THREAD_SIZE];
    enum thread_codes {
        WORK,
        COMPLETED
    } stthread[THREAD_SIZE];
    for(int i = 0; i < THREAD_SIZE; ++i) stthread[i] = WORK;

    for(int i = 0; i < THREAD_SIZE; ++i)
        threads[i] = new std::thread([&img, &c0, &window, &stthread](int cluster){
            std::cout << cluster << '\n';
            stthread[cluster] = WORK;
            while(window.isOpen())
            {
                if(stthread[cluster] != WORK) std::this_thread::sleep_for(std::chrono::milliseconds(10));

                for(int x = cluster; x < W_X; x += THREAD_SIZE)
                    for(int y = 0; y < W_Y; ++y)
                    {
                        double z[2]{___get_x(x), ___get_y(y)};
                        double that[2] {___get_x(x), ___get_y(y)};

                        img.setPixel(x, y, sf::Color::Red);
                        for(int i = 0; i < MAX_ITERATION; ++i)
                        {
                          // Тут мы всё и рисуем.
                            square_complex(z);
                            plus_copmex(z, that);
                          // А тут уже считаем.
                          
                          //Да и так считали, что посчитали - github пососная помойка для всратеньких проектов.
                          //Поэтому это здесь.
                            if((z[0] * z[0] + z[1] * z[1]) >= 20)
                            {
                                int c =  255*i/MAX_ITERATION;
                                img.setPixel(x, y, sf::Color(c, c, c));
                                break;
                            }
                        }
                    }
                stthread[cluster] = COMPLETED;
            }
        }, i);

    window.setActive(false);
    std::thread renderThread([&]{
        sf::Clock timer;
        while(window.isOpen())
        {
            for(int i = 0; i < THREAD_SIZE;)
                if(stthread[i] != WORK) {++i;}
            alpha += 3*M_PI/180;
            c0[0] = b * sin(alpha);
            c0[1] = b * cos(alpha);
            // std::cout << alpha << "(" << (alpha*180/M_PI) << ")" << "\n\t" << c0[0] << "; " << c0[1] << '\n';;
            window.clear();
            texture.loadFromImage(img);
            sprite.setTexture(texture);
            window.draw(sprite);
            drawLine({0, W_Y/2}, {W_X, W_Y/2}, window);
            drawLine({W_X/2, 0}, {W_X/2, W_Y}, window);
            window.display();
            for(int i = 0; i < THREAD_SIZE; ++i)
                stthread[i] = WORK;
            // std::this_thread::sleep_for(std::chrono::milliseconds(20 - timer.getElapsedTime().asMilliseconds()));
        }
    });
    auto mousePos = sf::Mouse::getPosition();
    while(window.isOpen())
    {
        while(window.waitEvent(event))
        {
            if(event.type == sf::Event::Closed)
                return 1;
            if(event.type == sf::Event::KeyPressed)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                {
                    offset.y += 0.05;
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    offset.y -= 0.05;
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    offset.x -= 0.05;
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    offset.x += 0.05;
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::U))
                {
                    scale += 0.05;
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::I))
                {
                    scale -= 0.05;
                    continue;
                }
            }
        }
    }
    return 0;
}

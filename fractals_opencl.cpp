#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include "math.h"
#include <fstream>
#include <mutex>
#include "utils.cpp"

const size_t W_X = 1920;
const size_t W_Y = 1920;

static double scale = 0.2;
sf::Vector2<double> offset{0.0, -1.0};

namespace {
    double c0 = 0;
    double c1 = 0;
    double alpha = 0;
    const double b = 0.3891;
    
    double scale_step = 0.01;
    double offset_step = 0.05;
}

void fillCanvasPos(uint32_t* canvas);

int main()
{    
    setlocale(LC_ALL, "Russian");
    //@init
    sf::Event event;
    sf::Image img;
    img.create(W_X, W_Y, sf::Color::White);

    c0= b * sin(alpha);
    c1= b * cos(alpha);

    sf::Texture texture;
    sf::Sprite sprite;

    texture.loadFromImage(img);
    sprite.setTexture(texture);
    //-----------------


    //@device render init

    //@select device from user
    cl::Device rDevice;
    std::vector<cl::Device> shitapi_cl;
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        std::vector<cl::Device> devices;
        
        
        for(size_t i = 0, __index_device = 0; i < platforms.size(); ++i)
        {
            //ohh.. I see you have 256GB ram
            decltype(devices) sdevices;
            platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &sdevices);
            devices.insert(devices.end(), sdevices.begin(), sdevices.end());
            std::cout << platforms[i].getInfo<CL_PLATFORM_NAME>() << ": " << '\n';
            for(; __index_device < devices.size(); ++__index_device)
            {
                std::cout << "\t[" << __index_device << "] " << devices[__index_device].getInfo<CL_DEVICE_NAME>() <<
                        "- [" << devices[__index_device].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << " work group size; "<<
                        devices[__index_device].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()<<" max items]" << '\n';
            }
        }
        size_t id = 0;
        std::cout << ">[index]: ";
        std::cin >> id;
        rDevice = devices[id];
        shitapi_cl.push_back(devices[id]);
        std::cout << "Selected " << devices[id].getInfo<CL_DEVICE_NAME>() << " device.\n";
    }
    sys_log("Create context.");
    cl::Context context(rDevice);
    sys_log("Create CommandQueue.");
    cl::CommandQueue queue(context, rDevice); // квеве

    sys_log("Alloc canvas.");
    uint32_t* host_canvas = new uint32_t[W_X*W_Y];
    fillCanvasPos(host_canvas);
    // uint32_t* hc = new uint32_t[W_X*W_Y];
    cl::Buffer canvas(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, W_X * W_Y * 4, host_canvas);

    //-----------------
__buildShader:
    //@build program shader
    cl::Program::Sources source;
    std::string strSourceCode;
    {
        std::fstream sourceCode("shader.cl");
        if(!sourceCode.is_open())
        {
            sys_log("Can't open file shader. (shader.cl)");
            return -127;
        }
        strSourceCode.assign( (std::istreambuf_iterator<char>(sourceCode)),
                    (std::istreambuf_iterator<char>()));
        source.push_back(std::make_pair(strSourceCode.c_str(), strSourceCode.size()));
        for(const auto& e: source)
        {
            sys_log(e.first, "\t---\t", e.second);
        }
    }
    sys_log("Create program and build");
    cl::Program program(context, source);
    try {
        program.build(shitapi_cl);
    }
    catch (cl::Error err)
    {
        sys_log(err.what());
        std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(rDevice);
        std::cerr << log << std::endl;
        std::cout << "Repeat?: ";
        int asn;
        std::cin >> asn;
        if (asn != 0) goto __buildShader;
    }

    sys_log("Create kernel");
    
    cl::Kernel kernel(program, "SHADERMAIN");
    std::mutex kernelMutex;
    int maxIt = 0;

    kernel.setArg(0, canvas);
    kernel.setArg(1, (double)W_X);
    kernel.setArg(2, (double)W_Y);
    kernel.setArg(3, (double)scale);
    kernel.setArg(4, (double)offset.x);
    kernel.setArg(5, (double)offset.y);
    kernel.setArg(6, (double)c0);
    kernel.setArg(7, (double)c1);

    //------------------
    sf::RenderWindow window(sf::VideoMode(W_X, W_Y), "M", sf::Style::Fullscreen);
    window.setActive(false);
    
    //@render
    std::thread renderThread([&]{
        while(window.isOpen())
        {
            alpha += 3*M_PI/180;
            c0 = b * sin(alpha);
            c1 = b * cos(alpha);
            // std::cout << alpha << "(" << (alpha*180/M_PI) << ")" << "\n\t" << c0[0] << "; " << c0[1] << '\n';;
            fillCanvasPos(host_canvas);
            canvas = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, W_X * W_Y * 4, host_canvas);
            kernelMutex.lock();
            kernel.setArg(0, canvas);
            kernel.setArg(6, (double)c0);
            kernel.setArg(7, (double)c1);
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(W_X * W_Y), cl::NDRange(rDevice.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()));
            queue.finish();
            queue.enqueueReadBuffer(canvas, true, 0, (W_X*W_Y*4), host_canvas);
            kernelMutex.unlock();
            // for(int i = 0; i < W_X*W_Y; ++i)
            // {
            //     int x = host_canvas[i] >> 16;
            //     int y = host_canvas[i]^(x << 16);
            //     std::cout << x << " " << y << "\n";
            // }

            img.create(W_X, W_Y, (uint8_t*)host_canvas);
            // for(int i = 0; i < 10; ++i)
            // {
            //     auto* ptr = (uint8_t*)(host_canvas + i);
            //     std::cout << (int)ptr[0] << ' ' << (int)ptr[1] << ' ' << (int)ptr[2] << '\n';
            // }
            window.clear();
            texture.loadFromImage(img);
            sprite.setTexture(texture);
            window.draw(sprite);
            window.display();
        }
    });

    //-----------------
    
    //@events
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
                    std::lock_guard<std::mutex> __(kernelMutex);
                    offset.y += offset_step;
                    kernel.setArg(5, (double)offset.y);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    std::lock_guard<std::mutex> __(kernelMutex);
                    offset.y -= offset_step;
                    kernel.setArg(5, (double)offset.y);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {
                    std::lock_guard<std::mutex> __(kernelMutex);
                    offset.x += offset_step;
                    kernel.setArg(4, (double)offset.x);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    std::lock_guard<std::mutex> __(kernelMutex);
                    offset.x -= offset_step;
                    kernel.setArg(4, (double)offset.x);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::U))
                {
                    std::lock_guard<std::mutex> __(kernelMutex);
                    scale += scale_step;
                    kernel.setArg(3, (double)scale);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::I))
                {
                    std::lock_guard<std::mutex> __(kernelMutex);
                    scale -= scale_step;
                    kernel.setArg(3, (double)scale);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9))
                {
                    scale_step += 5;
                    std::cout << scale_step << "\n";
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
                {
                    scale_step -= 5;
                    std::cout << scale_step << "\n";
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad7))
                {
                    offset_step += 0.01;
                    std::cout << offset_step << "\n";
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
                {
                    offset_step -= 0.01;
                    std::cout << offset_step << "\n";
                    continue;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5))
                {
                    std::cin >> offset_step >> scale_step;
                    std::cout << offset_step  << " - " << scale_step << "\n";
                    continue;
                }
            }
        }
    }
    //-----------------
    return 0;
}

void fillCanvasPos(uint32_t* canvas)
{
    for(int x = 0, i = 0; x < W_X; ++x)
        for(int y = 0; y < W_Y; ++y, ++i)
            canvas[i] = (y << 16) | x;
}

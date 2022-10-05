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
#include "engine/util.cpp"
#include <SFML/Window/VideoMode.hpp>

void fillCanvasPos(uint32_t* canvas);

size_t W_X = 1080;
size_t W_Y = 1920;

#define MULTIPLICITY_JULIA

static double scale = 0.2;
sf::Vector2<double> offset{0.0, 0.0};

namespace {
#ifdef MULTIPLICITY_JULIA
    double c0 = 0;
    double c1 = 0;
    double alpha = 0;
    const double b = 0.3891;
#endif
    double scale_step = 0.01;
    double offset_step = 0.05;
    size_t FRAMES_BUFFER = 1;
}

int main(int argc, char** argv)
{    

    //just for info
    bool showExtensions = argc >= 2;
    int MAX_ITERATIONS = 25;

    setlocale(LC_ALL, "Russian");
    //@init
    sf::Event event;
    sf::Image img;
#ifdef MULTIPLICITY_JULIA
    c0= b * sin(alpha);
    c1= b * cos(alpha);
#endif
    sf::Texture texture;
    sf::Sprite sprite;


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
                auto items = devices[__index_device].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
                std::cout << "\t[" << __index_device << "] " << devices[__index_device].getInfo<CL_DEVICE_NAME>() <<
                        " - [" << devices[__index_device].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << " work group size; ["<<
                        items[0] << "," << items[1] << "," << items[2] <<"] max items]\n";
                if(showExtensions)
                {
                    std::cout << "\textensions:\n\t\t";
                    auto stringExtensions = devices[__index_device].getInfo<CL_DEVICE_EXTENSIONS>();

                    for(const auto& c: stringExtensions)
                    {
                        if(c == ' ')
                            std::cout << "\n\t\t";
                        else
                            std::cout << c;
                    }

                    std::cout << '\n'; 
                }
            }
        }
        size_t id = 0;
        std::cout << ">[index]: ";
        std::cin >> id;
        rDevice = devices[id];
        shitapi_cl.push_back(devices[id]);
        std::cout << "Selected " << devices[id].getInfo<CL_DEVICE_NAME>() << " device.\n";
    }

    std::cout << "Array frame size[Bruh function. Only for testing.]: ";
    std::cin >> FRAMES_BUFFER;
    sf::VideoMode Mode;
    {
        auto modes = sf::VideoMode::getFullscreenModes();

        Mode = modes[0];
        W_X = Mode.width;
        W_Y = Mode.height;
        img.create(W_X, W_Y, sf::Color::White);
        texture.loadFromImage(img);
        sprite.setTexture(texture);
    }

    sf::Texture tinterface;
    sf::Sprite sinterface;
{        
    sf::Image iinterface;
    iinterface.create(
        W_X * 0.6,
        W_Y * 0.6,
        sf::Color(0)
    );
    for(int x = 0; x < (W_X * 0.6); ++x)
    {   
        iinterface.setPixel(x, 0, sf::Color(0xff7f));
        iinterface.setPixel(x, (W_Y * 0.6) - 1, sf::Color(0xff7f));
    }
    for(int y = 1; y < ((W_Y * 0.6)-1); ++y)
    {   
        iinterface.setPixel(0, y, sf::Color(0xff7f));
        iinterface.setPixel((W_X * 0.6)-1, y, sf::Color(0xff7f));
    }
    tinterface.loadFromImage(iinterface);
    sinterface.setTexture(tinterface);
}

    sys_log("Create context.");
    cl::Context context(rDevice);
    sys_log("Create CommandQueue.");
    cl::CommandQueue queue(context, rDevice); // квеве

    sys_log("Alloc canvas.");
    uint32_t* host_canvas = new uint32_t[W_X*W_Y*FRAMES_BUFFER];
    // uint32_t* hc = new uint32_t[W_X*W_Y];
    cl::Buffer canvas(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, W_X * W_Y * 4 * FRAMES_BUFFER, host_canvas);

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

    kernel.setArg(0, canvas);
    kernel.setArg(1, (double)W_X);
    // kernel.setArg(2, (double)W_Y);
    kernel.setArg(2, (double)scale);
    kernel.setArg(3, (double)offset.x);
    kernel.setArg(4, (double)offset.y);
    kernel.setArg(5, MAX_ITERATIONS);
#ifdef MULTIPLICITY_JULIA
    kernel.setArg(6, (double)c0);
    kernel.setArg(7, (double)c1);

#endif
    //------------------
    sf::RenderWindow window(Mode, "M", sf::Style::Fullscreen);
    window.setActive(false);

    std::mutex pipeline_mutex;
    std::vector<const sf::Drawable*> pipeline;
    pipeline.reserve(256);
    
    pipeline.emplace_back(&sprite);
    pipeline.emplace_back(&sinterface);
    //@render
    std::thread renderThread([&]{
        sf::Clock timer;
        while(window.isOpen())
        {
            try{
                timer.restart();
                fillCanvasPos(host_canvas);
                queue.enqueueWriteBuffer(canvas, false, 0, W_X*W_Y*4*FRAMES_BUFFER, host_canvas);
                kernelMutex.lock();
                for(int frame_idx = 0; frame_idx < FRAMES_BUFFER; ++frame_idx)
                {
                    unsigned* canvas_frame = host_canvas + (W_X*W_Y*frame_idx);
    #ifdef MULTIPLICITY_JULIA
                    alpha += 0.5*M_PI/180;
                    c0 = b * sin(alpha);
                    c1 = b * cos(alpha);
    #endif
                    // std::cout << alpha << "(" << (alpha*180/M_PI) << ")" << "\n\t" << c0[0] << "; " << c0[1] << '\n';;
                    // canvas = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, W_X * W_Y * 4, host_canvas);
                    // kernel.setArg(0, canvas);
    #ifdef MULTIPLICITY_JULIA
                    kernel.setArg(6, (double)c0);
                    kernel.setArg(7, (double)c1);
    #endif
                    queue.enqueueNDRangeKernel( kernel,
                                                cl::NDRange(0),
                                                cl::NDRange(W_Y * W_X));
                }
                kernelMutex.unlock();
                queue.enqueueReadBuffer(canvas, true, 0, (W_X*W_Y*4*FRAMES_BUFFER), host_canvas);
                queue.finish(); // wait all frames

                // frame execution
                for(int fx = 0; fx < FRAMES_BUFFER; ++fx)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(30 - timer.getElapsedTime().asMilliseconds()));
                    timer.restart();
                    // aliasing(host_canvas);
                    texture.update((uint8_t*)(host_canvas + (W_X*W_Y*fx)), W_X, W_Y, 0, 0);
                    sprite.setTexture(texture);
                    std::lock_guard<std::mutex> r(pipeline_mutex);
                }
            } catch(cl::Error err) {
                std::cout << err.what() << err.err() << '\n';
            }
        }
    });

    std::thread renderInterface([&] {
        sf::Clock timer;
        while (window.isOpen())
        {
            window.clear();
            for(const auto* dw: pipeline)
            {
                std::lock_guard<std::mutex> _(pipeline_mutex);
                window.draw(*dw);
            }
            window.display();
            std::this_thread::sleep_for(std::chrono::microseconds(30 - timer.getElapsedTime().asMilliseconds()));
            timer.restart();
        }
    });

    //-----------------
    
    //@events
    while(window.isOpen())
    {
        while(window.waitEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                    return 1;
                case sf::Event::MouseWheelScrolled:
                {
                    std::lock_guard<std::mutex> __(kernelMutex);
                    MAX_ITERATIONS += (event.mouseWheelScroll.delta);
                    kernel.setArg(5, MAX_ITERATIONS);
                    std::cout << MAX_ITERATIONS << '\n';
                    continue;
                } break;
                case sf::Event::MouseMoved:
                {
                    auto mv = sf::Mouse::getPosition(window);
                    sinterface.setPosition(mv.x - W_X * 0.3, mv.y - W_Y * 0.3);
                } break;
                case sf::Event::MouseButtonPressed:
                {
                    auto mv = sf::Mouse::getPosition(window);
                    offset.x -= ((double)mv.x/(double)W_X - 0.5)/(double)scale;
                    offset.y -= ((double)mv.y/(double)W_Y - 0.5)/(double)scale;
                    if(event.key.code == sf::Mouse::Left)
                    {
                        scale *= 1.6;
                    } else {
                        scale /= 1.6;
                    }

                    std::lock_guard<std::mutex> __(kernelMutex);
                    kernel.setArg(2, (double)scale);
                    kernel.setArg(3, (double)offset.x);
                    kernel.setArg(4, (double)offset.y);
                } break;
                case sf::Event::KeyPressed:
                {
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.y += offset_step;
                        kernel.setArg(4, (double)offset.y);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.y -= offset_step;
                        kernel.setArg(4, (double)offset.y);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.x += offset_step;
                        kernel.setArg(3, (double)offset.x);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.x -= offset_step;
                        kernel.setArg(3, (double)offset.x);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::U))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        scale += scale_step;
                        kernel.setArg(2, (double)scale);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::I))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        scale -= scale_step;
                        kernel.setArg(2, (double)scale);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9))
                    {
                        scale_step += 5;
                        std::cout << scale_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
                    {
                        scale_step -= 5;
                        std::cout << scale_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad7))
                    {
                        offset_step += 0.01;
                        std::cout << offset_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
                    {
                        offset_step -= 0.01;
                        std::cout << offset_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5))
                    {
                        std::cin >> offset_step >> scale_step;
                        std::cout << offset_step  << " - " << scale_step << "\n";
                        break;
                    }
                    if(event.key.code == sf::Keyboard::F10)
                    {
                        std::lock_guard<std::mutex> _(pipeline_mutex);
                        bool dontExists = true;
                        for(auto it = pipeline.begin(); it != pipeline.end(); ++it)
                        {
                            if((*it) == (&sinterface))
                            {
                                pipeline.erase(it);
                                *it = 0;
                                dontExists = false;
                                break;
                            }
                        }
                        if(dontExists)
                            pipeline.emplace_back(&sinterface);
                    }
                    if(event.key.code == sf::Keyboard::K)
                    {
                        std::cout   << "-----------------------\nOFFSET: " << offset.x << "; " << offset.y 
                                    << "\nSCALE: " << scale
                                    << "\nOFFSET AND SCALE STEP: " << offset_step << "; " << scale_step
                                    << "\nMOUSE POS " << sf::Mouse::getPosition(window).x << " " << sf::Mouse::getPosition(window).y
                                    <<"\n-----------------------\n"; 
                    } break;
                } break;
            }
        }
    }
    //-----------------
    return 0;
}

void fillCanvasPos(uint32_t* canvas)
{
    for(int canvas_idx = 0; canvas_idx < FRAMES_BUFFER; ++canvas_idx, canvas += W_X*W_Y)
        for(int y = 0, i = 0; y < W_Y; ++y)
            for(int x = 0; x < W_X; ++x, ++i)
                canvas[i] = (x << 16) | y;
}

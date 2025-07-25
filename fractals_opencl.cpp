#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#define __CL_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <fstream>
#include <mutex>
#include <SFML/Window/VideoMode.hpp>

// ----------DEFINES---------
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
    const double angle = 0.5*M_PI/180;
}

#ifdef DEBUG
#include <iostream>
#include <string>
template<class... Args>
inline void sys_log(Args&&... args)
{
    auto t = {
        (std::cout << args, 0)...
    };

    (void)t;
    std::cout << "\n";
}
#else
#define sys_log(x) /* x */
#endif
//---------- END D ---------------

void fillCanvasPos(uint32_t* canvas);

size_t W_X = 1080;
size_t W_Y = 1920;
bool freeze = false;


int main(int argc, char** argv)
{    

    //just for info
    bool showExtensions = argc >= 2;
    int32_t MAX_ITERATIONS = 25;

    setlocale(LC_ALL, "Russian");
    //@init
    sf::Image img;
#ifdef MULTIPLICITY_JULIA
    c0= b * sin(alpha);
    c1= b * cos(alpha);
#endif
    sf::Texture texture;


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
        if (devices.size() > 1)
        {
            size_t id = 0;
            std::cout << ">[index]: ";
            std::cin >> id;
            rDevice = devices[id];
            shitapi_cl.push_back(rDevice);
        } else {
            rDevice = devices[0];
            shitapi_cl.push_back(rDevice);
        }
        std::cout << "Selected " << rDevice.getInfo<CL_DEVICE_NAME>() << " device.\n";
    }

    sf::VideoMode Mode;
    
    auto modes = sf::VideoMode::getFullscreenModes();

    Mode = modes[0];
    W_X = (size_t)Mode.size.x;
    W_Y = (size_t)Mode.size.y;
    img.resize({(unsigned)W_X, (unsigned)W_Y});
    texture.loadFromImage(img);
    sf::Sprite sprite(texture);
        
    

    sf::Texture tinterface;
       
    sf::Image iinterface;
    iinterface.resize(
        {(unsigned)((float)W_X * 0.6),
        (unsigned)((float)W_Y * 0.6)},
        sf::Color(0)
    );
    for(int x = 0; x < (W_X * 0.6); ++x)
    {   
        iinterface.setPixel({(unsigned)x, 0}, sf::Color(0xff7f));
        iinterface.setPixel({(unsigned)x, (unsigned)(W_Y * 0.6) - 1}, sf::Color(0xff7f));
    }
    for(int y = 1; y < ((W_Y * 0.6)-1); ++y)
    {   
        iinterface.setPixel({0, (unsigned)y}, sf::Color(0xff7f));
        iinterface.setPixel({(unsigned)(W_X * 0.6)-1, (unsigned)y}, sf::Color(0xff7f));
    }
    tinterface.loadFromImage(iinterface);
    sf::Sprite sinterface(tinterface);


    std::cout << "Create context.\n";
    cl::Context context(rDevice);
    std::cout << "Create CommandQueue.\n";
    cl::CommandQueue queue(context, rDevice); // квеве

    std::cout << "Alloc canvas.\n";
    uint32_t* host_canvas = new uint32_t[W_X*W_Y];
    // uint32_t* hc = new uint32_t[W_X*W_Y];
    cl::Buffer canvas(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, W_X * W_Y * 4, host_canvas);

    //-----------------
__buildShader:
    //@build program shader
    cl::Program::Sources source;
    std::string strSourceCode;
    {
        std::fstream sourceCode("shader.cl");
        if(sourceCode.fail())
        {
            std::cout << "Can't open file shader. (shader.cl)";
            return -127;
        }
        strSourceCode.assign( (std::istreambuf_iterator<char>(sourceCode)),
                    (std::istreambuf_iterator<char>()));
        source.push_back(strSourceCode.c_str());
        for(const auto& e: source)
        {
            sys_log(e.first, "\t---\t", e.second);
        }
    }
    std::cout << "Create program and build\n";
    cl::Program program(context, source);
    try {
        program.build(shitapi_cl);
    }
    catch (cl::Error err)
    {
        std::cout << err.what();
        std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(rDevice);
        std::cerr << log << std::endl;
        std::cout << "Repeat?: ";
        int asn;
        std::cin >> asn;
        if (asn != 0) goto __buildShader;
    }

    std::cout << "Create kernel\n";
    
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
    sf::RenderWindow window(Mode, "M");
    window.setActive(false);

    std::mutex pipeline_mutex;
    std::vector<const sf::Drawable*> pipeline;
    pipeline.reserve(8);
    
    pipeline.emplace_back(&sprite);
    // pipeline.emplace_back(&sinterface);
    //@render
    std::thread renderThread([&]{
        sf::Clock timer;
        while(window.isOpen())
        {
            try{
                timer.restart();
                fillCanvasPos(host_canvas);
                queue.enqueueWriteBuffer(canvas, false, 0, W_X*W_Y*4, host_canvas);
                kernelMutex.lock();

                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                {
                    auto mv = sf::Mouse::getPosition(window);
                    offset.x -= 0.15*(((double)mv.x/(double)W_X - 0.5)/(double)scale);
                    offset.y -= 0.15*(((double)mv.y/(double)W_Y - 0.5)/(double)scale);
                    scale *= 1.036;
                    kernel.setArg(3, (double)offset.x);
                    kernel.setArg(4, (double)offset.y);
                } else if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) 
                {
                    auto mv = sf::Mouse::getPosition(window);
                    offset.x -= 0.3*(((double)mv.x/(double)W_X - 0.5)/(double)scale);
                    offset.y -= 0.3*(((double)mv.y/(double)W_Y - 0.5)/(double)scale);
                    scale /= 1.066;
                        kernel.setArg(3, (double)offset.x);
                    kernel.setArg(4, (double)offset.y);
                }
                

                kernel.setArg(2, (double)scale);
#ifdef MULTIPLICITY_JULIA
                if(!freeze)
                    alpha += angle;
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
                kernelMutex.unlock();
                queue.enqueueReadBuffer(canvas, true, 0, (W_X*W_Y*4), host_canvas);
                queue.finish(); // wait all frames

                // frame execution
                std::this_thread::sleep_for(std::chrono::milliseconds(30 - timer.getElapsedTime().asMilliseconds()));
                timer.restart();
                texture.update((uint8_t*)(host_canvas));
                sprite.setTexture(texture);
                
                std::lock_guard<std::mutex> r(pipeline_mutex);
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
            std::this_thread::sleep_for(std::chrono::milliseconds(30 - timer.getElapsedTime().asMilliseconds()));
            timer.restart();
        }
    });

    //-----------------
    
    //@events
    while(window.isOpen())
    {
        while(const std::optional event = window.waitEvent())
        {
            
            
                if(event->is<sf::Event::Closed>())
                    return 1;
                if(const auto* mouse = event->getIf<sf::Event::MouseWheelScrolled>())
                {
                    std::lock_guard<std::mutex> __(kernelMutex);
                    MAX_ITERATIONS += (mouse->delta);
                    kernel.setArg(5, MAX_ITERATIONS);
                    std::cout << MAX_ITERATIONS << '\n';
                    continue;
                }
                if(const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                {
                    auto mv = sf::Mouse::getPosition(window);
                    sinterface.setPosition({
                        (float)((float)mv.x - (float)W_X * 0.3),
                        (float)((float)mv.y - (float)W_Y * 0.3)});
                }
                if(event->is<sf::Event::MouseButtonPressed>())
                {
                    // auto mv = sf::Mouse::getPosition(window);
                    // offset.x -= (((double)mv.x/(double)W_X - 0.5)/(double)scale);
                    // offset.y -= (((double)mv.y/(double)W_Y - 0.5)/(double)scale);
                    // if(event.key.code == sf::Mouse::Left)
                    // {
                    //     scale *= 1.666;
                    // } else {
                    //     scale /= 1.666;
                    // }

                    // std::lock_guard<std::mutex> __(kernelMutex);
                    // kernel.setArg(2, (double)scale);
                    // kernel.setArg(3, (double)offset.x);
                    // kernel.setArg(4, (double)offset.y);
                }
                if(const auto* key = event->getIf< sf::Event::KeyPressed >())
                {
                    if(key->scancode == sf::Keyboard::Scan::Space)
                    {
                        freeze = !freeze;
                    }
                    if(key->scancode == sf::Keyboard::Scan::Left)
                    {
                        alpha -= angle;
                    }
                    if(key->scancode == sf::Keyboard::Scan::Right)
                    {
                        alpha += angle;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape))
                    {
                        return 0;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.y += offset_step;
                        kernel.setArg(4, (double)offset.y);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.y -= offset_step;
                        kernel.setArg(4, (double)offset.y);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.x += offset_step;
                        kernel.setArg(3, (double)offset.x);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        offset.x -= offset_step;
                        kernel.setArg(3, (double)offset.x);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::U))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        scale += scale_step;
                        kernel.setArg(2, (double)scale);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::I))
                    {
                        std::lock_guard<std::mutex> __(kernelMutex);
                        scale -= scale_step;
                        kernel.setArg(2, (double)scale);
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Numpad9))
                    {
                        scale_step += 5;
                        std::cout << scale_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Numpad3))
                    {
                        scale_step -= 5;
                        std::cout << scale_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Numpad7))
                    {
                        offset_step += 0.01;
                        std::cout << offset_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Numpad1))
                    {
                        offset_step -= 0.01;
                        std::cout << offset_step << "\n";
                        break;
                    }
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Numpad5))
                    {
                        std::cin >> offset_step >> scale_step;
                        std::cout << offset_step  << " - " << scale_step << "\n";
                        break;
                    }
                    if(key->scancode == sf::Keyboard::Scan::F10)
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
                    if(key->scancode == sf::Keyboard::Scan::K)
                    {
                        std::cout   << "-----------------------\nOFFSET: " << offset.x << "; " << offset.y 
                                    << "\nSCALE: " << scale
                                    << "\nOFFSET AND SCALE STEP: " << offset_step << "; " << scale_step
                                    << "\nMOUSE POS " << sf::Mouse::getPosition(window).x << " " << sf::Mouse::getPosition(window).y
                                    <<"\n-----------------------\n"; 
                    }
                } break;
        }
    }
    //-----------------
    return 0;
}

void fillCanvasPos(uint32_t* canvas)
{
    for(int y = 0, i = 0; y < W_Y; ++y)
        for(int x = 0; x < W_X; ++x, ++i)
            canvas[i] = (x << 16) | y;
}

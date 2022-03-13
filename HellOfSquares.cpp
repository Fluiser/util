#define WIN32
#include "system/Window.cpp"

int main()
{
    Engine::Window window;
    
    window.create(1280, 720, "MainWindow");
    while(!window.windowShouldClose())
    {
        slog("event...");
        window.waitEvent();
    }

    return 0;
}
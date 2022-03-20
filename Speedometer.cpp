#include "Speedometer.h"
#include "lvgl/lvgl.h"


Spedo::Spedo()
{
    lastTime = Clock::now();
    beginTime = lastTime;
    circumference = 2.0f;
    speed = 0.0f;
    rotCount = 0;

    
}

void Spedo::interrupt()
{
    rotCount++;
    std::chrono::time_point < std::chrono::high_resolution_clock> nowTime = Clock::now();
    //std::chrono::duration<float> dt = nowTime - lastTime;
    std::chrono::duration<float> dt = nowTime - lastTime;
    float f = dt.count();
    lastTime = nowTime;
    speed = circumference / f;
    speed = f;
    //std::cout << speed << "\n";
}

void Spedo::update()
{
    

}

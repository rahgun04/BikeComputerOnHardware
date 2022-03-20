#pragma once

#include <chrono>
#include <iostream>
#include <fstream>

typedef std::chrono::high_resolution_clock Clock;
using namespace std;

class Spedo {
public:
    

    std::chrono::time_point < std::chrono::high_resolution_clock> beginTime;
    std::chrono::time_point < std::chrono::high_resolution_clock> lastTime;

    std::chrono::duration<float> rideTime;

    float circumference;
    float speed;

    int secondsRide;
    int hoursRide;
    int minutesRide;
    uint64_t rotCount;


    fstream saveFile;
    



    Spedo();
    void interrupt();
    void update();
};

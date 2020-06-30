#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <cstring>
#include <chrono>
#include "drone.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <signal.h>
#include "pid.hpp"

std::atomic<bool> stopDrone(false);

void my_handler(int s){
    stopDrone = true;
}


int main()
{
    signal (SIGINT,my_handler);
    Drone d;
    d.takeOff();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    /*d.rc(0,50,0,0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    d.rc(0,-50,0,0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));*/
    const int maxSpeed = 30;
    const float maxSpeedf = 30.f;
    const int msBetweenCommands = 50;
    Pid PIDRotate = Pid(0.8, 0.01, 0, maxSpeed, -maxSpeed);
    Pid PIDForwardBackward = Pid(0.8, 0.01, 0.5, maxSpeed, -maxSpeed);
    Pid PIDLeftRight = Pid(0.8, 0.01, 0.5, maxSpeed, -maxSpeed);
    Pid PIDUpDown = Pid(0.8, 0.01, 0, maxSpeed, -maxSpeed);

    auto lastIteration = std::chrono::steady_clock::now();

    while(true){
        if (stopDrone){
            break;
        }
        float rotate = 0;
        float forwardBackward = 0;
        float leftRight = 0;
        float upDown = 0;
        {
            const std::lock_guard<std::mutex> lock(d.m_writeMutex);
            if (d.m_valuesSet){
                rotate = d.m_toRotate;
                forwardBackward = d.m_toGoForwardAndBackward;
                leftRight = d.m_toGoLeftAndRight;
                upDown = d.m_toGoUpAndDown;
            }
        }
        std::cout << "raw rotation: " << rotate << " fb: " << forwardBackward << " lr: " << leftRight << " ud: " << upDown << "\n";

        rotate =            limit(rotate,           -maxSpeedf, maxSpeedf);
        forwardBackward =   limit(forwardBackward,  -maxSpeedf, maxSpeedf);
        leftRight =         limit(leftRight,        -maxSpeedf, maxSpeedf);
        upDown =            limit(upDown,           -maxSpeedf, maxSpeedf);

        std::cout << "lim rotation: " << rotate << " fb: " << forwardBackward << " lr: " << leftRight << " ud: " << upDown << "\n";


        auto timeNow = std::chrono::steady_clock::now();
        //calculate how many seconds have passed, since last iteration, with milisecond precision
        //float timediff = (static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(lastIteration - timeNow).count()))/1000;
        float timediff = static_cast<float>(msBetweenCommands)/1000.f;
        std::array<int, 4> movement = {0,0,0,0};
        
        movement[0] =  PIDLeftRight.calculate(      0.f, leftRight, timediff);
        movement[1] = -PIDForwardBackward.calculate(0.f, forwardBackward, timediff);
        movement[2] = -PIDUpDown.calculate(         0.f, upDown, timediff);
        movement[3] = -PIDRotate.calculate(         0.f, rotate, timediff);
        std::cout << "inp rotation: " << movement[3] << " fb: " << movement[1] << " lr: " << movement[0] << " ud: " << movement[2] << "\n";

        std::cout << "\n\n";
        d.rc(movement[0], movement[1], movement[2], movement[3]);
        lastIteration = std::chrono::steady_clock::now();

        //give the camera thread time to write to the drone internal variables, otherwise lock would always be in use by this loop
        std::this_thread::sleep_for(std::chrono::milliseconds(msBetweenCommands));
    }
    d.land();
    std::cout << "Done\n";
}
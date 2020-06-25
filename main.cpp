#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <cstring>
#include <chrono>
#include "drone.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <signal.h>

std::atomic<bool> stopDrone(false);

void my_handler(int s){
    stopDrone = true;
}


int main()
{
    signal (SIGINT,my_handler);
    Drone d;
    d.takeOff();
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    //d.move(20,0,0,100);
    //d.move(-20,0,0,100);
    //d.move(0,20,0,100);
    //d.move(0,-20,0,100);
    //d.move(0,0,20,100);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //d.move(0,0,-20,100);
    while(true){
        if (stopDrone){
            break;
        }
        int rotate = 0;
        int forwardBackward = 0;
        int leftRight = 0;
        int upDown = 0;
        {
            const std::lock_guard<std::mutex> lock(d.m_writeMutex);
            rotate = d.m_toRotate;
            forwardBackward = d.m_toGoForwardAndBackward;
            leftRight = d.m_toGoLeftAndRight;
            upDown = d.m_toGoUpAndDown;
        }
        std::cout << "rotation: " << rotate << "\n";
        std::cout << "forward: " << forwardBackward << "\n";
        std::cout << "leftright: " << leftRight << "\n";
        std::cout << "updown: " << upDown << "\n";
        std::array<int, 3> movement = {0,0,0};
        if (rotate > 15 ||rotate < -15){
            d.rotate(rotate);
        } else {
            if (forwardBackward < -20 || forwardBackward > 20){
                movement[0] = forwardBackward;
            }
            if (leftRight < -20 || leftRight > 20){
                movement[1] = leftRight;
            }
            if (upDown < -20 || upDown > 20){
                movement[2] = upDown;
            }
            d.move(movement[0], movement[1], movement[2], 100);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    d.land();
    std::cout << "Done\n";
}
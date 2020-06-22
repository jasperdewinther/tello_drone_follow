#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <cstring>
#include <chrono>
#include "drone.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"





int main()
{
    Drone d;
    //d.takeOff();
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    //d.move(20,0,0,100);
    //d.move(-20,0,0,100);
    //d.move(0,20,0,100);
    //d.move(0,-20,0,100);
    //d.move(0,0,20,100);
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    //d.move(0,0,-20,100);
    std::this_thread::sleep_for(std::chrono::seconds(300));
    std::cout << "Done\n";
}
#pragma once
#include <SFML/Network.hpp>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/tracking.hpp>
#include <zbar.h>
#include <mutex>
#include "qrbb.hpp"

struct FlightParams{
    int m_rotate = 0;
    int m_lr = 0;
    int m_ud = 0;
    int m_fb = 0;
};

class DroneControls{
public:
    DroneControls(const int & lr, const int & fb, const int & ud, const int & rotate);
    FlightParams m_params;
};

class Drone{
    const sf::IpAddress m_sendIP = sf::IpAddress(192,168,10,1);
    const unsigned short m_sendPort = 8889;
    sf::IpAddress m_recvIP = sf::IpAddress(0,0,0,0);
    unsigned short m_recvPort = 9000;
    sf::UdpSocket m_sock;
    std::thread m_videoThread;
public:
    std::mutex m_writeMutex;
    float m_toRotate = 0;
    float m_toGoUpAndDown = 0;
    float m_toGoLeftAndRight = 0;
    float m_toGoForwardAndBackward = 0;
    bool m_valuesSet = false;

public:
    Drone();
    ~Drone();
private:
    void executeFunction(const std::string & command, const bool & waitForResponse = true);
public:
    void streamoff();
    void streamon();
    void getBattery();
    void takeOff();
    void land();
    void rotate(const int & rotation);
    void move(const int & fb, const int & lr, const int & ud, const int & speed);
    void rc(const int & lr, const int & fb, const int & ud, const int & yaw);
};
void getVideo(Drone * d);
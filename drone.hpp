#pragma once
#include <SFML/Network.hpp>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/tracking.hpp>
#include <zbar.h>

class Drone{
    const sf::IpAddress m_sendIP = sf::IpAddress(192,168,10,1);
    const unsigned short m_sendPort = 8889;
    sf::IpAddress m_recvIP = sf::IpAddress(0,0,0,0);
    unsigned short m_recvPort = 9000;
    sf::UdpSocket m_sock;
    std::thread m_videoThread;
    std::atomic<int> m_targetx;
    std::atomic<int> m_targety;
    std::atomic<int> m_targetw;
    std::atomic<int> m_targeth;

public:
    Drone();
    ~Drone();
private:
    void executeFunction(const std::string & command);
public:
    void streamoff();
    void streamon();
    void getBattery();
    void takeOff();
    void land();
    void move(const int & fb, const int & lr, const int & ud, const int & speed);
};
void getVideo(Drone * d);
void setRoi(std::atomic<double> x, std::atomic<double> y, std::atomic<double> w, std::atomic<double> h);
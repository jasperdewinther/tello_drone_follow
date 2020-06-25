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

class Qrbb{
public:
    Qrbb();
    Qrbb(const float & qrSize, const zbar::Symbol & symbol);
    Qrbb(const Qrbb & bb);
private:
    float qrSizeInCm;
	cv::Point bl;
	cv::Point tl;
	cv::Point tr;
	cv::Point br;
public:
	void draw(cv::Mat & frame) const;
    float getHorizontalDegree() const;
    float getVerticalDegree() const;
    float getDistanceInCm() const;
    cv::Point getCenter() const;
    float cmPerPixel() const;
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
    int m_toRotate = 0;
    int m_toGoUpAndDown = 0;
    int m_toGoLeftAndRight = 0;
    int m_toGoForwardAndBackward = 0;

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
    void rotate(const int & rotation);
    void move(const int & fb, const int & lr, const int & ud, const int & speed);
};
void getVideo(Drone * d);
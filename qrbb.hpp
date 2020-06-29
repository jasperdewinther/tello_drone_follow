#pragma once

#include <opencv2/opencv.hpp>
#include <zbar.h>



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
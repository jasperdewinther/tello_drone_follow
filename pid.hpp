#pragma once

#include <algorithm>
#include <iostream>


class Pid{
    float m_p;
    float m_i;
    float m_d;
    float m_maxOut;
    float m_minOut;
    float m_integral = 0;
    float m_prevError = 0;
public:
    Pid(const float & p, const float & i, const float & d, const float & maxOut, const float & minOut);
    float calculate(const float & target, const float & current, const float & td);
};

template <typename T> 
inline T limit(const T & value, const T & min, const T & max){
    return std::max(min, std::min(max, value));
}
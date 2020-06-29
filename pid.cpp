#include "pid.hpp"




Pid::Pid(const float & p, const float & i, const float & d, const float & maxOut, const float & minOut):
    m_p(p),
    m_i(i),
    m_d(d),
    m_maxOut(maxOut),
    m_minOut(minOut)
{}

float Pid::calculate(const float & target, const float & current, const float & dt){
    float error = target-current;

    float P = m_p * error;

    m_integral += error * dt;
    float I = m_i * m_integral;

    float derivative = (error - m_prevError) / dt;
    float D = m_d * derivative;

    float result = P + I + D;

    m_prevError = error;

    return limit(result, m_minOut, m_maxOut);
}
#pragma once

#include "plugin.hpp"

class SlewLimiter
{
    float deltaPitch = 0.0f;
    float out = 0.0f;
    float ramp = 0.0f;
    float riseAndFallTime = 0.0f;

public:
    void setVars(float riseAndFallTime)
    {
        this->riseAndFallTime = riseAndFallTime;
    }

    float process(float in, float sampleRate)
    {
        if (riseAndFallTime > 0.0f)
        {
            deltaPitch = fabsf(in - out);
            ramp = 1.0f / (1.0f + (riseAndFallTime / (1 + deltaPitch)) * sampleRate);

            if (in > out)
            {
                out += ramp;
                if (out >= in)
                {
                    out = in;
                }
            }
            else if (in < out)
            {
                out -= ramp;
                if (out <= in)
                {
                    out = in;
                }
            }
        }
        else
        {
            out = in;
        }

        return out;
    }
};
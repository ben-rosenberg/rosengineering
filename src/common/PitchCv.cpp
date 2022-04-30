#include "PitchCv.hpp"


void PitchCv::SetPitchArray(float pitchArray[], unsigned short int scaleNumber, unsigned short int noteNumber)
{
    const float noteOffset = noteNumber / 12.0f;
    pitchArray = {};

    const int scaleLength = scaleNumber < 2 ? 7 : 3;

    for (int i = 0; i < scaleLength; ++i) {
        pitchArray[i] = 
    }
}
#pragma once
#include <cmath>

class PitchCv
{
public:
    void SetPitchArray(float pitchArray[], unsigned short int scaleNumber, unsigned short int noteOffset);
    //float* operator[](int index);

private:
    const float m_ChromaticVoltages[12] = {
        0.0f,
        0.08333333582f,
        0.1666666716f,
        0.25f,
        0.3333333433f,
        0.4166666567f,
        0.5f,
        0.5833333135f,
        0.6666666865f,
        0.75f,
        0.8333333135f,
        0.9166666865F
        };

    const float m_MinorVoltages[7] = {
        0.0f,
        0.1666666716f,
        0.25f,
        0.4166666567f,
        0.5833333135f,
        0.6666666865f,
        0.8333333135f
    };

    const float m_MajorVoltages[7] = {
        0.0f,
        0.1666666716f,
        0.3333333433f,
        0.4166666567f,
        0.5833333135f,
        0.75f,
        0.9166666865f
    };

};

class Scale
{
public:
    Scale(const int scaleNumber, const int relativeNoteNumber)
    {
        const float noteOffset = relativeNoteNumber / 12.0f;
        int scaleLength = scaleNumber < 2 ? 7 : 3;

        for (int i = 0; i < scaleLength; ++i) {
            int noteNumber = i;
            if (scaleNumber == 0 && i )
            m_NoteArray[i] = std::fabs((i / 12.0f) + noteOffset);
        }
    }

    float operator[](const int relativeNoteNumber)
    {
        switch(relativeNoteNumber)
        {
            case 0: return m_Root;
            case 1: return m_FlatSecond;
            case 2: return m_Second;
        }
    }

private:
    float m_Root; // 0
    float m_FlatSecond; // 1
    float m_Second; // 2
    float m_MinorThird; // 3
    float m_MajorThird; // 4
    float m_Fourth; // 5
    float m_FlatFifth; // 6
    float m_Fifth; // 7
    float m_MinorSixth; // 8
    float m_MajorSixth; // 9
    float m_MinorSeventh; // 10
    float m_MajorSeventh; // 11 

    float m_NoteArray[12]{};
};
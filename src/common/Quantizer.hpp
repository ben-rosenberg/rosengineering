#pragma once
#include "../plugin.hpp"

/**
 * @class Quantizer
 * @brief Takes an input voltage an quantizes it to a given scale.
 *
 *
 */
// template <typename T>
class Quantizer
{
public:
    Quantizer()
        : m_RoundedNote(0.0f),
          m_Remainder(0),
          m_RescaledRemainder(0),
          m_outputVoltage(0.0f),
          m_LastOutputVoltage(0.0f),
          m_Gate(0.0f)
    {
    }

    enum SCALES
    {
        MINOR,
        MAJOR,
        MELODIC_MINOR_ASCENDING,
        HARMONIC_MINOR,
        MINOR_TRIAD,
        MAJOR_TRIAD
    };

    void setAllowedNotes(int scale)
    {
        switch (scale)
        {
        case 0: // minor
            m_allowed[0] = 0,
            m_allowed[1] = 2,
            m_allowed[2] = 3,
            m_allowed[3] = 5,
            m_allowed[4] = 7,
            m_allowed[5] = 8,
            m_allowed[6] = 10;
            break;

        case 1: // major
            m_allowed[0] = 0,
            m_allowed[1] = 2,
            m_allowed[2] = 4,
            m_allowed[3] = 5,
            m_allowed[4] = 7,
            m_allowed[5] = 9,
            m_allowed[6] = 11;
            break;

        case 2: // melodic minor ascending
            m_allowed[0] = 0,
            m_allowed[1] = 2,
            m_allowed[2] = 3,
            m_allowed[3] = 5,
            m_allowed[4] = 7,
            m_allowed[5] = 9,
            m_allowed[6] = 11;
            break;

        case 3: // harmonic minor
            m_allowed[0] = 0,
            m_allowed[1] = 2,
            m_allowed[2] = 3,
            m_allowed[3] = 5,
            m_allowed[4] = 7,
            m_allowed[5] = 8,
            m_allowed[6] = 11;
            break;

        case 4: // minor triad
            m_allowed[0] = 0,
            m_allowed[1] = 0,
            m_allowed[2] = 3,
            m_allowed[3] = 3,
            m_allowed[4] = 7,
            m_allowed[5] = 7,
            m_allowed[6] = 0;
            break;

        case 5: // major triad
            m_allowed[0] = 0,
            m_allowed[1] = 0,
            m_allowed[2] = 4,
            m_allowed[3] = 4,
            m_allowed[4] = 7,
            m_allowed[5] = 7,
            m_allowed[6] = 0;
            break;
        }
    }

    /**
     * First multiplies input voltage by 12, rounds that to the nearest whole value,
     * and divides by 12 to give the nearest note. m_Remainder stores how many semitones
     * above the root note the rounded note is. This remainder is then rescaled from 0->12
     * to 0->7 for use in the following function.
     *
     * @param inPitch the input voltage multiplied by range CV and parameter.
     */
    inline void semitoneRound(float inPitch)
    {
        m_RoundedNote = simd::round(12.0f * inPitch) / 12.0f;
        m_Remainder = (static_cast<int>(simd::round(m_RoundedNote * 12)) + 60) % 12;
        m_RescaledRemainder = rescale(m_Remainder, 0, 12, 0, 7);
    }

    /**
     * Iterates through the "allowed" notes in the chosen scale, testing to see if the
     * current rounded note already matches one of the allowed notes. If it does, the current
     * semitone-rounded voltage is output. If it does not, the remainder is subtracted from
     * the nearest allowed note and then divided by 12. This is then added to current rounded
     * note, yielding the nearest allowed note.
     *
     * @return the quantized value to be output.
     */
    float setOutputVoltage()
    {
        for (int a = 0; a < 7; ++a)
        {
            if (m_Remainder == m_allowed[a])
            {
                m_outputVoltage = m_RoundedNote;
            }

            if (m_outputVoltage != m_RoundedNote)
            {
                m_outputVoltage = m_RoundedNote + ((m_allowed[m_RescaledRemainder] - m_Remainder) / 12.0f);
            }
        }
        return m_outputVoltage;
    }

    float generateGate()
    {
        m_Gate = 10.0f;
        m_Gate *= (m_LastOutputVoltage != m_outputVoltage) & 1;

        return m_Gate;
    }

private:
    int m_allowed[7];
    float m_RoundedNote;
    int m_Remainder;
    int m_RescaledRemainder;
    float m_outputVoltage;
    float m_LastOutputVoltage;
    float m_Gate;
};

class Quantizer2
{
public:
    Quantizer2() : m_RoundedNote(0.0f),
                   m_Remainder(0),
                   m_RescaledRemainder(0),
                   m_outputVoltage(0.0f),
                   m_LastOutputVoltage(0.0f),
                   m_Gate(0.0f)
    {
    }

    void setAllowedNotes(bool C, bool C_sharp, bool D, bool D_sharp, bool E, bool F, bool F_sharp,
                         bool G, bool G_sharp, bool A, bool A_sharp, bool B)
    {
        const int cAllowed = 0;
        const int cSharpAllowed = 1;
        const int dAllowed = 2;
        const int dSharpAllowed = 3;
        const int eAllowed = 4;
        const int fAllowed = 5;
        const int fSharpAllowed = 6;
        const int gAllowed = 7;
        const int gSharpAllowed = 8;
        const int aAllowed = 9;
        const int aSharpAllowed = 10;
        const int bAllowed = 11;

        const int possibleNotes[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

        for (int i = 0; i < 12; ++i)
        {
            m_allowed[i] = 0;
        }
    }

    /**
     * First multiplies input voltage by 12, rounds that to the nearest whole value,
     * and divides by 12 to give the nearest note. m_Remainder stores how many semitones
     * above the root note the rounded note is. This remainder is then rescaled from 0->12
     * to 0->7 for use in the following function.
     *
     * Converts the input CV from voltage to nearest semitone by multiplying by 12 and
     * rounding that value to the nearest number. CV is 1V/oct, and 1 oct is 12 semitones.
     * This rounded value is then converted back to voltage for some reason by dividing by 12.
     *
     * @param inPitch the input voltage multiplied by range CV and parameter.
     */
    inline void semitoneRound(float inPitch)
    {
        m_RoundedNote = simd::round(12.0f * inPitch) / 12.0f;
        m_Remainder = (static_cast<int>(simd::round(m_RoundedNote * 12)) + 60) % 12;
        m_RescaledRemainder = rescale(m_Remainder, 0, 12, 0, 7);
    }

    /**
     * Same as the first version except the theoretically unnecessary dividing
     * and multiplying by 12 in the first two lines is removed. Will it work?
     * God only knows. ACTUALLY I think it needs to be that way for the next function.
     * There's a better way to do all this. Consolidate functions yo.
     *
     * @param inPitch the input voltage multiplied by range CV and parameter.
     */
    inline void semitoneRoundNew(float inPitch)
    {
        m_RoundedNote = simd::round(12.0f * inPitch);
        m_Remainder = (static_cast<int>(simd::round(m_RoundedNote)) + 60) % 12;
        m_RescaledRemainder = rescale(m_Remainder, 0, 12, 0, 7);
    }

    /**
     * Iterates through the "allowed" notes in the chosen scale, testing to see if the
     * current rounded note already matches one of the allowed notes. If it does, the current
     * semitone-rounded voltage is output. If it does not, the remainder is subtracted from
     * the nearest allowed note and then divided by 12. This is then added to current rounded
     * note, yielding the nearest allowed note.
     *
     * @return the quantized value to be output.
     */
    float setOutputVoltage()
    {
        for (int a = 0; a < 12; ++a)
        {
            if (m_Remainder == m_allowed[a])
            {
                m_outputVoltage = m_RoundedNote;
            }

            if (m_outputVoltage != m_RoundedNote)
            {
                m_outputVoltage = m_RoundedNote + ((m_allowed[m_RescaledRemainder] - m_Remainder) / 12.0f);
            }
        }
        return m_outputVoltage;
    }

    /**
     * @brief why is the & 1 necessary? I don't think it is, but I gotta
     * brush up on my bitwise knowledge.
     *
     * @return float
     */
    float generateGate()
    {
        m_Gate = 10.0f;
        m_Gate *= (m_LastOutputVoltage != m_outputVoltage) & 1;

        return m_Gate;
    }

private:
    int m_allowed[12];
    bool m_AllowedBool[12];
    float m_RoundedNote;
    int m_Remainder;
    int m_RescaledRemainder;
    float m_outputVoltage;
    float m_LastOutputVoltage;
    float m_Gate;
};

class MajorOrMinorQuantizer
{
public:
    MajorOrMinorQuantizer()
        : m_RoundedNote(0.0f), m_Remainder(0), m_RescaledRemainder(0), m_OutputVoltage(0.0f), m_LastOutputVoltage(0.0f), m_Gate(0.0f)
    {
    }

    void setAllowedNotes(bool isMajor)
    {
        if (isMajor)
        {
            m_AllowedNotes[0] = 0;
            m_AllowedNotes[1] = 4;
            m_AllowedNotes[2] = 7;
        }
        else
        {
            m_AllowedNotes[0] = 0;
            m_AllowedNotes[1] = 3;
            m_AllowedNotes[2] = 7;
        }
    }

    void semitoneRound(float inPitch)
    {
        m_RoundedNote = simd::round(12.0f * inPitch) / 12.0f;
        m_Remainder = (static_cast<int>(simd::round(m_RoundedNote * 12)) + 60) % 12;
        m_RescaledRemainder = rescale(m_Remainder, 0, 12, 0, 7);
    }

    float setOutputVoltage()
    {
        for (int a = 0; a < 3; ++a)
        {
            if (m_Remainder == m_AllowedNotes[a])
            {
                m_OutputVoltage = m_RoundedNote;
            }

            if (m_OutputVoltage != m_RoundedNote)
            {
                m_OutputVoltage = m_RoundedNote + ((m_AllowedNotes[m_RescaledRemainder] - m_Remainder) / 12.0f);
            }
        }
        return m_OutputVoltage;
    }

private:
    uint16_t m_AllowedNotes[3] = {};
    float m_RoundedNote;
    int m_Remainder;
    int m_RescaledRemainder;
    float m_OutputVoltage;
    float m_LastOutputVoltage;
    float m_Gate;
};
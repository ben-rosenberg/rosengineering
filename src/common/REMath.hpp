//#pragma once
#ifndef TARMATH_H
#define TARMATH_H

#include <math.h>
#include <rack.hpp>

namespace TAR
{

namespace Math 
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/** CONSTANTS */
static const float e = 2.7182818f;

template <typename T>
class Vec2D
{
public:
    Vec2D()
        : m_X(0.0), m_Y(0.0)
    {
    }

    Vec2D(T x, T y)
        : m_X(x), m_Y(y)
    {
    }

    Vec2D(const TAR::Math::Vec2D<T>& other)
        : m_X(other.getX()), m_Y(other.getY())
    {
    }

    T getX() const { return m_X; }
    T getY() const { return m_Y; }

    operator rack::math::Vec() const
    {
        return rack::math::Vec(m_X, m_Y);
    }
private:
    T m_X;
    T m_Y;
};

/* =============================================================== */

template <typename T>
class Rectangle
{
public:
    Rectangle()
        : m_Position(0.0, 0.0), m_Dimensions(0.0, 0.0)
        , m_X(0.0), m_Y(0.0)
        , m_Width(0.0), m_Height(0.0)
    {
        for (size_t i = 0; i < 4; ++i)
            m_Points[i] = Vec2D<T>(0.0, 0.0);
    }

    Rectangle(Vec2D<T> position, Vec2D<T> dimensions)
        : m_Position(position), m_Dimensions(dimensions)
        , m_X(m_Position.getX()), m_Y(m_Position.getY())
        , m_Width(m_Dimensions.getX()), m_Height(m_Dimensions.getY())
    {
        m_Points[0] = m_Position;
        m_Points[1] = Vec2D<T>(m_X + m_Width, m_Y);
        m_Points[2] = Vec2D<T>(m_X + m_Width, m_Y - m_Height);
        m_Points[3] = Vec2D<T>(m_X, m_Y - m_Height);
    }

    operator rack::math::Rect() const
    {
        return rack::math::Rect((rack::math::Vec)m_Position, (rack::math::Vec)m_Dimensions);
    }
private:
    Vec2D<T> m_Position;
    Vec2D<T> m_Dimensions;

    T m_X;
    T m_Y;
    T m_Width;
    T m_Height;

    Vec2D<T> m_Points[4];
};

/* template <typename T>
struct Vec16
{
public:
    Vec16()
        : container[]{};
    {

    }
    Vec16(
        T first, T second, T third, T fourth, 
        T fifth, T sixth, T seventh, T eighth, 
        T ninth, T tenth, T eleventh, T twelfth, 
        T thirteenth, T fourteenth, T fifteenth, T sixteenth )
    {   
        container = {
            first, second, third, fourth, fifth, sixth, seventh, eighth, 
            ninth, tenth, eleventh, twelfth, thirteenth, fourteenth, fifteenth, sixteenth };   
    }

private:
    T container[16];
}; */

/**
 * @brief Rescales a value. Templated to accept any numerical type.
 * 
 * @tparam The type of value to be rescaled.
 * @param input The value to be rescaled.
 * @param currentMin The current minimum value of the input.
 * @param max The current maximum value of the input.
 * @param desiredMin The desired minimum value of the output.
 * @param desiredMax The desired maximum value of the output.
 * @return T The rescaled value.
 */
template <typename T = float>
T rescale(T input, T currentMin, T max, T desiredMin, T desiredMax)
{
    T minDifference = desiredMin - currentMin;
    T currentRange = max - currentMin;
    T desiredRange = desiredMax - desiredMin;
    T rangeRatio = desiredRange / currentRange;
    return input * rangeRatio + minDifference;
}


/**
 * @brief Crossfades between two values.
 * 
 * Not useful for most dry/wet mixing contexts, as the dry input will fade to \n
 * half its amplitude as the mix parameter value approaches 0.5 instead of staying \n 
 * at 1.
 * 
 * @tparam T The type of values to be mixed. 
 * @param x The first input. "Dry" component in a dry/wet context.
 * @param y The second input. "Wet" component in a dry/wet context.
 * @param mixParam The crossfading parameter. Expected to be between 0 and 1.
 * @return T The crossfaded mix.
 */
template <typename T>
T crossfade(T x, T y, float mixParam)
{
    T xMultiplier = 0.0f;
    T yMultiplier = 0.0f;
    
    /** "Flips" the mixParam values such that when between 0 and 0.5, they TODO */ 
    xMultiplier = rescale<T>(mixParam, 0.0f, 1.0f, 1.0f, 0.0f);
    yMultiplier = mixParam;

    return (x * (T)xMultiplier + y * (T)yMultiplier);
}


/**
 * @brief Crossfading for dry/wet effect blending scenarios
 * 
 * This version of the crossfade function is much more usable for dry/wet effect \n
 * mixes, as the dry input stays at full volume until the mix parameter's value \n
 * exceeds 0.5. The mix is defined using multipliers for the dry and wet. \n
 * 
 * First, we check to make sure the value of the mix parameter is between 0 and 1. \n 
 * If less than 0, the dry signal is returned, and if greater than 1, the wet \n
 * signal is returned. Then the case of 0 to 0.5 is handled by maintaining the \n
 * dry signal's full volume. When between 0.5 and 1.0, the dry signal is attenuated \n
 * by multiplying it by the value of the mix parameter that has been "flipped" \n
 * and multiplied by two. We multiply by two so that as the parameter value \n
 * increases from 0.5 to 1.0, the dry signal's multiplier decreases from 1.0 \n
 * to 0.0. Without the multiplication by 2, the dry signal will suddenly jump \n
 * to half its amplitude as soon as the mix parameter value exceeds 0.5. \n
 * Finally, the wet signal's multiplier is defined to equal the mix parameter. \n
 * 
 * @param dry The dry component.
 * @param wet The wet component.
 * @param mixParam The parameter controlling the mixture of the two signals.
 * @return float The crossfaded signal.
 */
//template <typename T>
inline float dryWetCrossfade(float dry, float wet, float mixParam)
{
    if (mixParam < 0.0f)
        return dry;
    if (mixParam > 1.0f)
        return wet;

    float dryMultiplier = 0.0f;
    float wetMultiplier = 0.0f;

    if (mixParam >= 0.0f && mixParam <= 0.5f)
        dryMultiplier = 1.0f;
    else if(mixParam > 0.5f && mixParam <= 1.0f)
        dryMultiplier = rescale<float>(mixParam, 0.0f, 1.0f, 1.0f, 0.0f) * 2;

    wetMultiplier = mixParam;

    return (dry * dryMultiplier + wet * wetMultiplier);
}

template <typename T>
T sign(T x)
{
    return ((-1 * (T)(x < 0)) + (T)(x > 0));
}

// Works except for -0
template <typename T>
T absValue(T x)
{
    return (x * sign<T>(x));
}

// this is the dumbest shit, but it does work.
template <typename T>
T findSmallestDistanceToValue(T inputArray[], T value, const int numElements)
{
    T smallestDistanceSoFar = absValue<T>(inputArray[0]) - absValue<T>(value);
    int indexOfSmallestDistanceSoFar = 0;

    for (int i = 0; i < numElements; ++i)
    {
        float currentElementDistance = absValue<T>(*(inputArray + i) - absValue<T>(value));
        if (currentElementDistance < smallestDistanceSoFar)
        {
            smallestDistanceSoFar = currentElementDistance;
            indexOfSmallestDistanceSoFar = i;
        }
        smallestDistanceSoFar = (currentElementDistance < smallestDistanceSoFar) ? currentElementDistance : smallestDistanceSoFar;
    }

    return indexOfSmallestDistanceSoFar;
}


template <typename T>
T sigmoidFunction(T x)
{
    return (1 / (1 + std::pow(e, -x)));
}

template <typename T>
T sine(T x)
{
    return T(sse_mathfun_sin_ps(x));
}

template <typename T>
T clamp(T x, T min, T max)
{
    T clampedX = 0.0f;

	if (x >= min && x <= max)
		clampedX = x;
	if (x < min)
		clampedX = min;
	if (x > max)
		clampedX = max;

	return clampedX;
}

// Learn about floats at the bit level to implement this!
/* template <typename T>
T floor(T x)
{
    if (x - 0.5f < x)
}

template <typename T>
T round(T x)
{
    T y = x + 1;
    T z = x - 1;

    T delta = 0.1;
    int counter = 0;


    while (x < (x + 1))
    {
        x += delta;
        ++counter;
    }

    if (x - 0.5f < x)
        return (x % 
} */

template <typename T>
T getMax8(T first, T second, T third, T fourth, 
           T fifth, T sixth, T seventh, T eighth)
{
    T max;

    max = (first > second) ? first : second;
    max = (max > third)   ? max : third;
    max = (max > fourth)  ? max : fourth;
    max = (max > fifth)   ? max : fifth;
    max = (max > sixth)   ? max : sixth;
    max = (max > seventh) ? max : seventh;
    max = (max > eighth)  ? max : eighth;

    return max;
}

}
}
#endif
#pragma once

#include "plugin.hpp"
#include "common/SequencerWidget.hpp"
#include "common/RosEngineering.hpp"
#include "common/REComponents.hpp"
#include "common/Quantizer.hpp"
#include "common/SlewLimiter.hpp"

class TuningModulator
{
public:
    TuningModulator();

    float randomSquareLFO(bool gate);
    float sineLFO(float pitch, float sampleTime, float pitchAttenuation);
    float randomSmoothLFO(float currentValue, float lastValue, float phase);

private:
    float m_Phase;
};

struct SemitoneSequencer : Module
{
    enum ParamIds
    {
        OCT1_PARAM,
        OCT2_PARAM,
        OCT3_PARAM,
        OCT4_PARAM,
        OCT5_PARAM,
        OCT6_PARAM,
        OCT7_PARAM,
        OCT8_PARAM,
        ENUMS(FAKE_OCT_PARAM, 32),
        SEMITONE1_PARAM,
        SEMITONE2_PARAM,
        SEMITONE3_PARAM,
        SEMITONE4_PARAM,
        SEMITONE5_PARAM,
        SEMITONE6_PARAM,
        SEMITONE7_PARAM,
        SEMITONE8_PARAM,
        ENUMS(FAKE_SEMITONE_PARAM, 32),
        CLOCK_PARAM,
        MODE_SWITCH_PARAM,
        MEASURE_SWITCH_PARAM,
        RESET_PARAM,
        RUNNING_PARAM,
        DETUNE_AMOUNT_PARAM,
        SWING_PARAM,
        GATE_PROBABILITY_PARAM,
        SLIDE_PARAM,

        STEP1_ACTIVE_PARAM,
        STEP2_ACTIVE_PARAM,
        STEP3_ACTIVE_PARAM,
        STEP4_ACTIVE_PARAM,
        STEP5_ACTIVE_PARAM,
        STEP6_ACTIVE_PARAM,
        STEP7_ACTIVE_PARAM,
        STEP8_ACTIVE_PARAM,
        ENUMS(FAKE_STEP_ACTIVE_PARAM, 32),

        NUM_PARAMS
    };
    enum InputIds
    {
        RESET_INPUT,
        CLOCK_INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        GATE_OUTPUT,
        CV_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        CURRENT_LIGHT1_LIGHT,
        ACTIVE_LIGHT1_LIGHT,
        CURRENT_LIGHT2_LIGHT,
        ACTIVE_LIGHT2_LIGHT,
        CURRENT_LIGHT3_LIGHT,
        ACTIVE_LIGHT3_LIGHT,
        CURRENT_LIGHT4_LIGHT,
        ACTIVE_LIGHT4_LIGHT,
        CURRENT_LIGHT5_LIGHT,
        ACTIVE_LIGHT5_LIGHT,
        CURRENT_LIGHT6_LIGHT,
        ACTIVE_LIGHT6_LIGHT,
        CURRENT_LIGHT7_LIGHT,
        ACTIVE_LIGHT7_LIGHT,
        CURRENT_LIGHT8_LIGHT,
        ACTIVE_LIGHT8_LIGHT,
        CURRENT_MEASURE1_LIGHT,
        MEASURE1_LIGHT,
        CURRENT_MEASURE2_LIGHT,
        MEASURE2_LIGHT,
        CURRENT_MEASURE3_LIGHT,
        MEASURE3_LIGHT,
        CURRENT_MEASURE4_LIGHT,
        MEASURE4_LIGHT,
        STEP1_ACTIVE_LIGHT,
        STEP2_ACTIVE_LIGHT,
        STEP3_ACTIVE_LIGHT,
        STEP4_ACTIVE_LIGHT,
        STEP5_ACTIVE_LIGHT,
        STEP6_ACTIVE_LIGHT,
        STEP7_ACTIVE_LIGHT,
        STEP8_ACTIVE_LIGHT,
        RESET_LIGHT,
        GATE_LIGHT,
        RUNNING_LIGHT,
        NUM_LIGHTS
    };

    /** Member variables and objects, mainly to be used in the process function. */
    enum SequencerMode
    {
        Forward,
        Reverse,
        Random
    };
    enum DetuneMode
    {
        RANDOM_GATE,
        LFO,
        RANDOM_LFO
    };
    dsp::SchmittTrigger trigger, resetTrigger, runningTrigger, randSqrTrigger, stepActiveTrigger;
    dsp::BooleanTrigger swingTrigger;
    dsp::PulseGenerator changedMeasureNumberPulse;
    dsp::Timer switchTimer, lengthTimer;
    // dsp::ExponentialSlewLimiter slewLimiter;
    // dsp::SlewLimiter slewLimiter[4];
    SlewLimiter slew[4];
    bool running = true;
    bool gate = false;
    int numStepsPerMeasure = 0;
    int numMeasures = 69;
    int stepNumber = 0;
    int numStepsToIncrement = 1;
    int measureNumber = 0;
    float phase = 0.f;
    int detuneMode = RANDOM_GATE;
    SequencerMode seqMode = Forward;
    int chordMode = 0;
    int quantizationMode = 0;
    int lastMeasureSwitch = 0;
    int lastMeasureNumber = 0;
    int numToUseForMeasureLights = 0;
    TuningModulator lfo[4];
    float randValue = 0.0f;
    float randLFOValue = 0.0f;
    float randLFOLastValue = 0.0f;
    Quantizer quantizer;

    /**
     * This overrides the display values for the mode param such that
     * they display these strings instead of the default float values.
     */
    struct Mode : ParamQuantity
    {
        std::string getDisplayValueString() override
        {
            switch ((int)getValue())
            {
            case 0:
                return "Forward";
                break;
            case 1:
                return "Reverse";
                break;
            case 2:
                return "Random";
                break;
            default:
                return "";
                break;
            }
        }
    };

    /** Just like the Mode struct, except for the running param. */
    struct Running : ParamQuantity
    {
        bool runningLabel = true;
        std::string getDisplayValueString() override
        {
            return ((isMax()) ? "Running" : "Stopped");
        }
    };

    /** Just like the Mode struct, except for the Reset param. */
    struct ResetSwitch : ParamQuantity
    {
        std::string getDisplayValueString() override
        {
            return ((getValue() > 0.0f) ? "Reset" : "");
        }
    };

    /**  */
    struct SlideParam : ParamQuantity
    {
        std::string getDisplayValueString() override
        {
            return ((getValue() >= 99.9999999f) ? "OFF" : std::to_string(getValue()));
        }
    };

    struct SwingParam : ParamQuantity
    {
        std::string getDisplayValueString() override
        {
            return std::to_string(getValue()) + " : 1";
        }
    };

    struct StepActiveParam : ParamQuantity
    {
        std::string getDisplayValueString() override;
    };

    SemitoneSequencer();

    void onReset() override;
    void onRandomize() override;
    void onAdd() override;

    json_t *dataToJson() override;
    void dataFromJson(json_t *) override;

    void setStep(int, int);
    void resetRandLFO();

    void process(const ProcessArgs &) override;
};

#include <string>
#include <vector>
#include "SemitoneSequencer.hpp"

/**
 * @struct TuningModulator
 * @brief handles detuning of the CV output.
 *
 * This struct is responsible for all three detuning modes.
 */

TuningModulator::TuningModulator() : m_Phase(0.0f){};

float TuningModulator::randomSquareLFO(bool gate)
{
    if (gate)
    {
        return random::uniform() * 2.0f - 1.0f;
    }
    else
        return 0.0f;
}

float TuningModulator::sineLFO(float pitch, float sampleTime, float pitchAttenuation)
{
    float freq = 0.5f * std::pow(2.0f, pitch * pitchAttenuation);
    m_Phase += freq * sampleTime;
    if (m_Phase >= 0.5f)
        m_Phase -= 1.0f;
    return simd::sin(2.0f * M_PI * m_Phase);
}

float TuningModulator::randomSmoothLFO(float currentValue, float lastValue, float phase)
{
    float v = 0;
    v = std::fmin(phase, 1.f);
    v = std::cos(M_PI * v);
    v = rescale(v, 1.0f, -1.0f, lastValue, currentValue);
    return v * 2.0f - 1.0f;
}

struct CVSlider
{
    float durationInSamples = 0;

    float slide(float nextNote, float currentNote, float sampleTime, float slideSpeed)
    {
        float cvDifference = nextNote - currentNote;
        float stepDuration = (60.0f / slideSpeed);
        durationInSamples = stepDuration / sampleTime;
        float CVToAddPerSample = cvDifference / durationInSamples;
        return CVToAddPerSample;
    }
};

/**
 * @struct SemitoneSequencer
 * @brief the main module class where IO and params are configured.
 *
 * This is the main class for the module. It inherits from Rack's Module class.
 * Inputs, outputs, and parameters are declared and initialized, and parameters
 * are configured. It also contains the essential process function where compon-
 * ents, including all I/O, are processed.
 */

std::string SemitoneSequencer::StepActiveParam::getDisplayValueString()
{
    if (getValue())
        return "On";
    else
        return "Off";
}

/** Constructor */
SemitoneSequencer::SemitoneSequencer()
{
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    for (int i = 0; i < 8; i++)
    {
        std::string stepString = std::to_string(i + 1);
        configParam(OCT1_PARAM + i, -4.f, 4.f, 0.f, "Step " + stepString + " octave", "", 0.f, 1.f, 4.f);
        configParam(SEMITONE1_PARAM + i, 0.f, 11.f, 0.f, "Step " + stepString + " semitone");
        configParam /*<StepActiveParam>*/ (STEP1_ACTIVE_PARAM + i, 0.0f, 1.0f, 1.0f, "Step " + stepString);
    }
    configParam(CLOCK_PARAM, -2.f, 6.f, 2.f, "Clock", " BPM", 2.f, 60.f);
    configParam<Mode>(MODE_SWITCH_PARAM, 0.f, 2.f, 0.f, "Mode");
    configParam(MEASURE_SWITCH_PARAM, 0.f, 3.f, 0.f, "Measure", "", 0.0f, 1.0f, 1.0f);
    configParam<ResetSwitch>(RESET_PARAM, 0.0f, 1.0f, 0.0f, "Reset");
    configParam<Running>(RUNNING_PARAM, 0.0f, 1.0f, 0.0f, "Run");
    configParam(DETUNE_AMOUNT_PARAM, 0.0f, 1.0f, 0.0f, "Detune amount", " cents", 0.0f, 100.0f);
    // configParam(LFO_RATE_ATTEN_PARAM, 0.0f, 2.0f, 1.0f, "LFO frequency multiplier", "%", 0.0f, 100.0f);
    configParam(SWING_PARAM, 1.0f, 10.0f, 1.0f, "Swing ratio");
    configParam(GATE_PROBABILITY_PARAM, 0.0f, 1.0f, 1.0f, "Gate probability", "%", 0.0f, 100.0f);
    configParam /* <SlideParam> */ (SLIDE_PARAM, 0.0f, 1.0f, 0.0f, "Slide rate", "", 0.0f, 1.0f);
    for (int i = 0; i < 32; ++i)
    {
        configParam(FAKE_OCT_PARAM + i, -4.f, 4.f, 0.f, "");
        configParam(FAKE_SEMITONE_PARAM + i, 0.f, 11.f, 0.f, "");
        configParam(FAKE_STEP_ACTIVE_PARAM + i, 0.0f, 1.0f, 1.0f, "");
    }
}

/**
 * When the module is initialized, this resets variables that aren't reset
 * by the call normally.
 */
void SemitoneSequencer::onReset()
{
    running = false;
    numStepsPerMeasure = 8;
    numMeasures = 4;
    chordMode = 0;
    detuneMode = RANDOM_GATE;
    quantizationMode = 0;
    stepNumber = 0;
    for (int i = 0; i < 32; ++i)
    {
        params[FAKE_OCT_PARAM + i].setValue(0.0f);
        params[FAKE_SEMITONE_PARAM + i].setValue(0.0f);
    }
}

/**
 * When the module is randomized, this randomizes variables that aren't randomized \n
 * normally. In particular, this randomizes the number of steps per measure and the \n
 * "fake" parameter values, because those aren't randomized by Rack for some reason.
 */
void SemitoneSequencer::onRandomize()
{
    numStepsPerMeasure = random::u32() % 8 + 1;
    for (int i = 0; i < 32; ++i)
    {
        params[FAKE_OCT_PARAM + i].setValue(std::round(random::uniform() * 8.0f) - 4.0f);
        params[FAKE_SEMITONE_PARAM + i].setValue(random::u32() % 11);
    }
    params[MEASURE_SWITCH_PARAM].setValue(0.0f);
    numMeasures = random::u32() % 4 + 1;
    stepNumber = 0;
    measureNumber = 0;
    quantizationMode = 0;
}

/**
 * Similar to the previous two functions, this initializes all the variables that \n
 * are necessary for functionality but which aren't tied to parameters, meaning they \n
 * are not automatically initialized. This avoids several critical bugs that otherwise \n
 * result in crashes.
 */
void SemitoneSequencer::onAdd()
{
    if (numMeasures == 69)
    {
        running = false;
        numStepsPerMeasure = 8;
        numMeasures = 4;
        detuneMode = RANDOM_GATE;
        quantizationMode = 0;
    }
}

/** Stores non-parameter variables in the json file. */
json_t *SemitoneSequencer::dataToJson()
{
    json_t *rootJ = json_object();

    /** Running */
    json_object_set_new(rootJ, "running", json_boolean(running));

    /** Current step number */
    json_object_set_new(rootJ, "current_step_number", json_integer(stepNumber));

    /** Number of steps per measure*/
    json_object_set_new(rootJ, "Number of Steps", json_integer(numStepsPerMeasure));

    /** Number of measures */
    json_object_set_new(rootJ, "number_of_measures", json_integer(numMeasures));

    /** Chord mode */
    json_object_set_new(rootJ, "chord_mode", json_integer(chordMode));

    /** Detune mode */
    json_object_set_new(rootJ, "detune_mode", json_integer((int)detuneMode));

    /** Quantization mode */
    json_object_set_new(rootJ, "quantization_mode", json_integer(quantizationMode));

    return rootJ;
}

/** Reads the non-parameter functionality variables from the json file. */
void SemitoneSequencer::dataFromJson(json_t *rootJ)
{
    /** Running */
    json_t *runningJ = json_object_get(rootJ, "running");
    if (runningJ)
        running = json_is_true(runningJ);

    /** Current step number */
    json_t *stepNumberJ = json_object_get(rootJ, "current_step_number");
    if (stepNumberJ)
        stepNumber = json_integer_value(stepNumberJ);

    /** Number of steps per measure */
    json_t *numStepsJ = json_object_get(rootJ, "Number of Steps");
    if (numStepsJ)
        numStepsPerMeasure = json_integer_value(numStepsJ);

    /** Number of measures */
    json_t *measuresJ = json_object_get(rootJ, "number_of_measures");
    if (measuresJ)
        numMeasures = json_integer_value(measuresJ);

    /** Chord mode */
    json_t *chordModeJ = json_object_get(rootJ, "chord_mode");
    if (chordModeJ)
        chordMode = json_integer_value(chordModeJ);

    /** Detuned mode */
    json_t *detuneModeJ = json_object_get(rootJ, "detune_mode");
    if (detuneModeJ)
        detuneMode = json_integer_value(detuneModeJ);

    /** Detuned mode */
    json_t *quantizationModeJ = json_object_get(rootJ, "quantization_mode");
    if (quantizationModeJ)
        quantizationMode = json_integer_value(quantizationModeJ);
}

/**
 * Resets the step to prevent it from advancing to a non-existent step.
 *
 * @param stepNumber is the current step number, from 0 - 31.
 * @param mode is the mode. Forward, reverse, or random.
 */
void SemitoneSequencer::setStep(int stepNumber, int mode)
{
    phase = 0.f;
    this->stepNumber = stepNumber;
    if (seqMode == Forward)
    {
        if (this->stepNumber >= (numStepsPerMeasure * numMeasures))
            this->stepNumber = 0;
    }
    else if (seqMode == Reverse)
    {
        if (this->stepNumber < 0)
            this->stepNumber = (numStepsPerMeasure * numMeasures) - 1;
    }
    else
    {
        this->stepNumber = random::u32() % ((numStepsPerMeasure * numMeasures) - 1);
    }
}

/** Resets the random LFO values. Needs work. */
void SemitoneSequencer::resetRandLFO()
{
    randLFOLastValue = randLFOValue;
}

/** ... */
void SemitoneSequencer::process(const ProcessArgs &args)
{
    seqMode = SequencerMode(params[MODE_SWITCH_PARAM].getValue());
    randLFOValue = random::uniform();

    /**
     * Calculates the measure number based on the step number and number of \n
     * steps per measure.
     */
    if ((stepNumber < numStepsPerMeasure))
        measureNumber = 0;
    else if ((stepNumber >= numStepsPerMeasure) && (stepNumber < (numStepsPerMeasure * 2)))
        measureNumber = 1;
    else if (stepNumber >= (numStepsPerMeasure * 2) && (stepNumber) < (numStepsPerMeasure * 3))
        measureNumber = 2;
    else if (stepNumber >= numStepsPerMeasure * 3)
        measureNumber = 3;

    /**
     * Allows the current measure to dictate the value of the measure switch \n
     * while running. This also makes possible the displayed knob values not \n
     * to change when playback is stopped until the measure switch is manually \n
     * changed. Also creates the measureSwitch variable.
     */
    if (running)
        params[MEASURE_SWITCH_PARAM].setValue(measureNumber);
    int measureSwitch = (int)params[MEASURE_SWITCH_PARAM].getValue();

    int firstNoteInMeasure = numStepsPerMeasure * measureNumber;
    int lastNoteInMeasure = (numStepsPerMeasure - 1) * measureNumber;
    if (stepNumber == lastNoteInMeasure)
        switchTimer.process(args.sampleTime);
    if (stepNumber == firstNoteInMeasure)
        switchTimer.reset();

    // maybe avoid timing
    if (switchTimer.process(args.sampleTime) > 0.0f)
        changedMeasureNumberPulse.trigger(10);
    else
        changedMeasureNumberPulse.reset();

    int measureValueToUse = (running) ? measureNumber : measureSwitch;
    bool writeOn = changedMeasureNumberPulse.process(args.sampleTime) || !running;
    if (writeOn && lastMeasureSwitch == measureSwitch)
    {
        for (int step = 0; step < 8; ++step)
        {
            params[FAKE_OCT_PARAM + step + measureValueToUse * 8].setValue(
                params[OCT1_PARAM + step].getValue());
            params[FAKE_SEMITONE_PARAM + step + measureValueToUse * 8].setValue(
                params[SEMITONE1_PARAM + step].getValue());
            params[FAKE_STEP_ACTIVE_PARAM + step + measureValueToUse * 8].setValue(
                params[STEP1_ACTIVE_PARAM + step].getValue());
        }
    }

    /** Populate two arrays with the values of the fake params. */
    float fakeOctParamValues[32] = {};
    float fakeSemitoneParamValues[32] = {};
    float fakeStepActiveParamValues[32] = {};
    for (int i = 0; i < 32; ++i)
    {
        fakeOctParamValues[i] = params[FAKE_OCT_PARAM + i].getValue();
        fakeSemitoneParamValues[i] = params[FAKE_SEMITONE_PARAM + i].getValue();
        fakeStepActiveParamValues[i] = params[FAKE_STEP_ACTIVE_PARAM + i].getValue();
    }

    /** Turn running on and off when the running button is pressed. */
    if (runningTrigger.process(params[RUNNING_PARAM].getValue()))
        running = !running;

    /**  */
    if (running)
    {
        for (int step = 0; step < 8; ++step)
        {
            params[OCT1_PARAM + step].setValue(
                fakeOctParamValues[step + measureNumber * 8]);
            params[SEMITONE1_PARAM + step].setValue(
                fakeSemitoneParamValues[step + measureNumber * 8]);
            params[STEP1_ACTIVE_PARAM + step].setValue(
                fakeStepActiveParamValues[step + measureNumber * 8]);
        }

        /** External clock */
        if (inputs[CLOCK_INPUT].isConnected())
        {
            if (trigger.process(inputs[CLOCK_INPUT].getVoltage()))
            {
                /**
                 * Pass to the setStep function the current step number plus 1 \n
                 * if the mode is forward or random, and pass it the step number \n
                 * minus 1 if it is reverse.
                 */
                if (seqMode == Forward || seqMode == Random)
                {
                    while (!(fakeStepActiveParamValues[stepNumber + numStepsToIncrement]))
                        ++numStepsToIncrement;

                    setStep(stepNumber + 1, seqMode);
                    numStepsToIncrement = 1;
                }
                else
                {
                    while (!(fakeStepActiveParamValues[stepNumber - numStepsToIncrement]))
                        ++numStepsToIncrement;

                    setStep(stepNumber - 1, seqMode);
                    numStepsToIncrement = 1;
                }

                randValue = random::uniform();
                resetRandLFO();
            }

            /** Sets the gate according to the clock trigger */
            gate = trigger.isHigh();
        }
        /** Internal clock */
        else
        {
            if (params[SWING_PARAM].getValue() > 1.0f)
            {
                swingTrigger.process(!(stepNumber % 2));
            }
            float swingMultiplier = (swingTrigger.state) ? params[SWING_PARAM].getValue() : 1;
            float clockTime = std::pow(2.f, params[CLOCK_PARAM].getValue() * swingMultiplier) / (params[SWING_PARAM].getValue());
            phase += clockTime * args.sampleTime;
            if (phase >= 1.f)
            {
                if (seqMode == Forward || seqMode == Random)
                {
                    while (!(fakeStepActiveParamValues[stepNumber + numStepsToIncrement]))
                        ++numStepsToIncrement;

                    setStep(stepNumber + 1, seqMode);
                    numStepsToIncrement = 1;
                }
                else
                {
                    while (!(fakeStepActiveParamValues[stepNumber - numStepsToIncrement]))
                        ++numStepsToIncrement;

                    setStep(stepNumber - numStepsToIncrement, seqMode);
                    numStepsToIncrement = 1;
                }
                randValue = random::uniform();
                resetRandLFO();
            }

            /** Sets the gate according to the phase */
            gate = (phase < 0.5f);
        }
    }
    else
    {
        for (int step = 0; step < 8; ++step)
        {
            params[OCT1_PARAM + step].setValue(
                fakeOctParamValues[step + measureSwitch * 8]);
            params[SEMITONE1_PARAM + step].setValue(
                fakeSemitoneParamValues[step + measureSwitch * 8]);
            params[STEP1_ACTIVE_PARAM].setValue(
                fakeStepActiveParamValues[step + measureSwitch * 8]);
        }
    }

    /** Sets the gate output. */
    bool randomizedGate = false;
    if (params[GATE_PROBABILITY_PARAM].getValue() > 0.0f)
        randomizedGate = gate && (randValue < params[GATE_PROBABILITY_PARAM].getValue());
    outputs[GATE_OUTPUT].setVoltage((randomizedGate) ? 10.0f : 0.0f);

    /** Takes care of variables responsible for pitch slidng. */
    float riseAndFall = params[SLIDE_PARAM].getValue();
    for (int i = 0; i < 4; ++i)
        slew[i].setVars(riseAndFall);

    /** ... */
    int outputIndex = stepNumber + (measureNumber * (8 - numStepsPerMeasure));

    if (quantizationMode <= 1)
        quantizer.setAllowedNotes(quantizer.MINOR);
    else if (quantizationMode == 2)
        quantizer.setAllowedNotes(quantizer.MAJOR);

    float LFOPitchAtten = params[SWING_PARAM].getValue();

    float monoOrPolyMeasureIndex = (chordMode == 0) ? 1 : numMeasures;
    /**
     * Sets the CV output, first for the normal monophonic mode, and then for the \n
     * chords mode.
     */
    if (running)
    {
        float rawPitch = 0.0f;
        float preSlewCV[4] = {};
        float outputCV[4] = {};
        /** Iterates through the channels / chord voices. */
        for (int c = 0; c < monoOrPolyMeasureIndex; ++c)
        {
            float detuneAmount[4] = {};
            if (params[DETUNE_AMOUNT_PARAM].getValue() > 0)
            {
                switch (detuneMode)
                {
                case RANDOM_GATE:
                    detuneAmount[c] = lfo[c].randomSquareLFO(gate);
                    break;
                case LFO:
                    detuneAmount[c] = lfo[c].sineLFO(
                        params[CLOCK_PARAM].getValue(), args.sampleTime, LFOPitchAtten);
                    break;
                case RANDOM_LFO:
                    detuneAmount[c] = lfo[c].randomSmoothLFO(
                        randLFOValue, randLFOLastValue, phase);
                    break;
                default:
                    detuneAmount[c] = 0.0f;
                }
                detuneAmount[c] *= params[DETUNE_AMOUNT_PARAM].getValue();
            }
            else
                detuneAmount[c] = 0.0f;

            outputIndex = (chordMode) ? ((stepNumber % numStepsPerMeasure) + (c * 8)) : (stepNumber + (measureNumber * (8 - numStepsPerMeasure)));
            rawPitch =
                fakeOctParamValues[outputIndex] +
                (fakeSemitoneParamValues[outputIndex] / 12.0f);

            if (quantizationMode == 0)
                preSlewCV[c] = rawPitch + detuneAmount[chordMode * c] / 12.0f;
            else
            {
                quantizer.semitoneRound(rawPitch);
                preSlewCV[c] = quantizer.setOutputVoltage() + detuneAmount[chordMode * c] / 12.0f;
            }

            if (params[SLIDE_PARAM].getValue() <= 0.0f)
                outputCV[c] = preSlewCV[c];
            else
                outputCV[c] = slew[c].process(preSlewCV[c], args.sampleRate);

            outputs[CV_OUTPUT].setChannels((chordMode > 0) ? numMeasures : 1);
            outputs[CV_OUTPUT].setVoltage(outputCV[c], c);
        }
    }

    /** Handles a reset call, either via the reset input or the reset button. */
    if (inputs[RESET_INPUT].getVoltage() + resetTrigger.process(params[RESET_PARAM].getValue()))
    {
        if (seqMode == Forward || seqMode == Random)
        {
            stepNumber = 0;
            measureNumber = 0;
            params[MEASURE_SWITCH_PARAM].setValue(0.0f);
        }
        else if (seqMode == Reverse)
        {
            stepNumber = numStepsPerMeasure * numMeasures - 1;
            measureNumber = numMeasures - 1;
            params[MEASURE_SWITCH_PARAM].setValue(static_cast<float>(numMeasures - 1));
        }
    }

    /** These if statements are for writing the measure lights. */
    if (running || resetTrigger.isHigh())
        numToUseForMeasureLights = measureNumber;
    else if (changedMeasureNumberPulse.process(args.sampleTime))
        numToUseForMeasureLights = measureSwitch;

    /* Set the step lights. */
    for (int i = 0; i < 8; i++)
    {
        float currentLightDisplayValue = 0.0f;
        float activeLightDisplayValue = 0.0f;
        float stepActiveLightDisplayValue = 0.0f;

        if ((i == stepNumber % numStepsPerMeasure))
            currentLightDisplayValue = 1.0f;
        else
            currentLightDisplayValue = 0.0f;

        if ((i != stepNumber % numStepsPerMeasure && i < numStepsPerMeasure) || (!running && measureNumber != measureSwitch))
        {
            activeLightDisplayValue = 0.25f;
            currentLightDisplayValue = 0.0f;
        }
        else
            activeLightDisplayValue = 0.0f;

        if (fakeStepActiveParamValues[i + numToUseForMeasureLights * 8])
            lights[STEP1_ACTIVE_LIGHT + i].setSmoothBrightness(1.0f, args.sampleTime);
        else
            lights[STEP1_ACTIVE_LIGHT + i].setSmoothBrightness(0.0f, args.sampleTime);

        lights[CURRENT_LIGHT1_LIGHT + 2 * i].setSmoothBrightness(
            currentLightDisplayValue, args.sampleTime);
        lights[ACTIVE_LIGHT1_LIGHT + 2 * i].setSmoothBrightness(
            activeLightDisplayValue, args.sampleTime);
    }

    /* Set the measure lights. */
    for (int i = 0; i < 4; ++i)
    {
        /** For the current measure */
        if (numToUseForMeasureLights == i)
        {
            lights[CURRENT_MEASURE1_LIGHT + 2 * i].setSmoothBrightness(1.0f, args.sampleTime);
            lights[MEASURE1_LIGHT + 2 * i].setSmoothBrightness(0.0f, args.sampleTime);
        }
        /** For the active, but not current measures. */
        else if (i < numMeasures && numToUseForMeasureLights != i)
        {
            lights[CURRENT_MEASURE1_LIGHT + 2 * i].setSmoothBrightness(0.0f, args.sampleTime);
            lights[MEASURE1_LIGHT + 2 * i].setSmoothBrightness(0.25f, args.sampleTime);
        }
        /** For inactive measures. */
        else
        {
            lights[CURRENT_MEASURE1_LIGHT + 2 * i].setSmoothBrightness(0.0f, args.sampleTime);
            lights[MEASURE1_LIGHT + 2 * i].setSmoothBrightness(0.0f, args.sampleTime);
        }
    }

    /* Set the running, reset, and clock lights. */
    lights[RESET_LIGHT].setSmoothBrightness(resetTrigger.isHigh(), args.sampleTime); // this is a little awkward
    lights[GATE_LIGHT].setSmoothBrightness(randomizedGate, args.sampleTime);
    lights[RUNNING_LIGHT].setSmoothBrightness(running, args.sampleTime);

    /* Resets the variables that track whether a param is changed. */
    lastMeasureSwitch = measureSwitch;
}
//};

/* struct ModeMenu : ui::Menu
{
    TAR::Math::Vec2D<float> position = TAR::Math::Vec2D(5.56f, 86.0f);
    TAR::Math::Vec2D<float> size = TAR::Math::Vec2D(17.5f, 5.0f);
    TAR::Math::Rectangle<float> boundingBox = TAR::Math::Rectangle(position, size);

    void draw() override
    {

    }



    //rack::Widget::box = (rack::math::Rect)boundingBox;
    //rack::math::Rect boundingBox = rack::math::Rect((rack::math::Vec)position, size);
}; */

/**
 * @struct SemitoneSequencerWidget
 * @brief Manages the module.
 *
 * Manages everything to do with the physical layout of the module. Where components
 * lie, context menu items, SVG's to be used for components, etc.
 */
struct SemitoneSequencerWidget : ModuleWidget
{
    void appendContextMenu(Menu *menu) override
    {
        /** From here to the constructor deals with context menu items. */
        SemitoneSequencer *module = dynamic_cast<SemitoneSequencer *>(this->module);

        /** Forward/reverse/random mode menu item. */
        menu->addChild(new MenuEntry);
        menu->addChild(createMenuLabel("Number of steps per measure"));
        struct ModeItem : MenuItem
        {
            SemitoneSequencer *module;
            int mode;
            void onAction(const event::Action &e) override
            {
                module->numStepsPerMeasure = mode + 1;
            }
        };

        std::string modeNames[8] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight"};
        for (int i = 0; i < 8; i++)
        {
            ModeItem *modeItem = createMenuItem<ModeItem>(modeNames[i]);
            modeItem->rightText = CHECKMARK(module->numStepsPerMeasure == i + 1);
            modeItem->module = module;
            modeItem->mode = i;
            menu->addChild(modeItem);
        }

        /** Number of measures / chord voices menu item. */
        menu->addChild(new MenuEntry);
        menu->addChild(createMenuLabel("Number of measures / chord voices"));
        struct MeasuresItem : MenuItem
        {
            SemitoneSequencer *module;
            int measures;
            void onAction(const event::Action &e) override
            {
                module->numMeasures = measures + 1;
            }
        };

        std::string measureNames[4] = {"One", "Two", "Three", "Four"};
        for (int i = 0; i < 4; i++)
        {
            MeasuresItem *measuresItem = createMenuItem<MeasuresItem>(measureNames[i]);
            measuresItem->rightText = CHECKMARK(module->numMeasures == i + 1);
            measuresItem->module = module;
            measuresItem->measures = i;
            menu->addChild(measuresItem);
        }

        /** Regular monophonic or chords mode menu item. */
        menu->addChild(new MenuEntry);
        menu->addChild(createMenuLabel("Chord mode"));
        struct ChordItem : MenuItem
        {
            SemitoneSequencer *module;
            int chordMode;
            void onAction(const event::Action &e) override
            {
                module->chordMode = chordMode;
            }
        };

        std::string chordModeNames[2] = {"Off", "On"};
        for (int i = 0; i < 2; ++i)
        {
            ChordItem *pItem = createMenuItem<ChordItem>(chordModeNames[i]);
            pItem->rightText = CHECKMARK(module->chordMode == i);
            pItem->module = module;
            pItem->chordMode = i;
            menu->addChild(pItem);
        }

        /** Detune mode */
        menu->addChild(new MenuEntry);
        menu->addChild(createMenuLabel("Detune mode"));
        struct DetuneModeItem : MenuItem
        {
            SemitoneSequencer *module;
            int detuneMode;
            void onAction(const event::Action &e) override
            {
                module->detuneMode = detuneMode;
            }
        };

        std::string detuneModeNames[3] = {"Random gate", "LFO", "Random LFO"};
        for (int i = 0; i < 3; ++i)
        {
            DetuneModeItem *dItem = createMenuItem<DetuneModeItem>(detuneModeNames[i]);
            dItem->rightText = CHECKMARK(module->detuneMode == i);
            dItem->module = module;
            dItem->detuneMode = i;
            menu->addChild(dItem);
        }

        /** Quantization mode */
        menu->addChild(new MenuEntry);
        menu->addChild(createMenuLabel("Quantization mode"));
        struct QuantizationModeItem : MenuItem
        {
            SemitoneSequencer *module;
            int quantizationMode;
            void onAction(const event::Action &e) override
            {
                module->quantizationMode = quantizationMode;
            }
        };

        std::string quantizationModeNames[3] = {"Off", "Minor", "Major"};
        for (int i = 0; i < 3; ++i)
        {
            QuantizationModeItem *qItem = createMenuItem<QuantizationModeItem>(quantizationModeNames[i]);
            qItem->rightText = CHECKMARK(module->quantizationMode == i);
            qItem->module = module;
            qItem->quantizationMode = i;
            menu->addChild(qItem);
        }
    }

    SemitoneSequencerWidget(SemitoneSequencer *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SemitoneSequencer.svg")));

        addChild(createWidget<TAR::Components::BlackScrew>(Vec(mm2px(5.08), 0)));                      // top
        addChild(createWidget<TAR::Components::BlackScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0))); // top
        addChild(createWidget<TAR::Components::BlackScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<TAR::Components::BlackScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(15.24, 21.82f - 5.08f)), module, SemitoneSequencer::OCT1_PARAM));
        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(15.24f, 42.14f - 5.08f)), module, SemitoneSequencer::OCT2_PARAM));
        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(15.24f, 62.46f - 5.08f)), module, SemitoneSequencer::OCT3_PARAM));
        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(15.24f, 82.78f - 5.08f)), module, SemitoneSequencer::OCT4_PARAM));
        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(42.97f, 21.82f - 5.08f)), module, SemitoneSequencer::OCT5_PARAM));
        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(42.97f, 42.14f - 5.08f)), module, SemitoneSequencer::OCT6_PARAM));
        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(42.97f, 62.46f - 5.08f)), module, SemitoneSequencer::OCT7_PARAM));
        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(42.97f, 82.78f - 5.08f)), module, SemitoneSequencer::OCT8_PARAM));

        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(27.94f, 21.82f - 5.08f)), module, SemitoneSequencer::SEMITONE1_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(27.94f, 42.14f - 5.08f)), module, SemitoneSequencer::SEMITONE2_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(27.94f, 62.46f - 5.08f)), module, SemitoneSequencer::SEMITONE3_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(27.94f, 82.78f - 5.08f)), module, SemitoneSequencer::SEMITONE4_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(55.88f, 21.82f - 5.08f)), module, SemitoneSequencer::SEMITONE5_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(55.88f, 42.14f - 5.08f)), module, SemitoneSequencer::SEMITONE6_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(55.88f, 62.46f - 5.08f)), module, SemitoneSequencer::SEMITONE7_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(55.88f, 82.78f - 5.08f)), module, SemitoneSequencer::SEMITONE8_PARAM));

        addParam(createParamCentered<TAR::Components::MyTrimpot>(mm2px(Vec(12.7f, 116.0f)), module, SemitoneSequencer::DETUNE_AMOUNT_PARAM));
        addParam(createParamCentered<TAR::Components::MyTrimpot>(mm2px(Vec(27.94f, 116.0f)), module, SemitoneSequencer::GATE_PROBABILITY_PARAM));
        addParam(createParamCentered<TAR::Components::MyTrimpot>(mm2px(Vec(43.18f, 116.0f)), module, SemitoneSequencer::SWING_PARAM));
        addParam(createParamCentered<TAR::Components::MyTrimpot>(mm2px(Vec(58.42f, 116.0f)), module, SemitoneSequencer::SLIDE_PARAM));

        addParam(createParamCentered<TAR::Components::MySnapTrimpot>(mm2px(Vec(50.8f, 92.94f - 2.54f)), module, SemitoneSequencer::MEASURE_SWITCH_PARAM));
        addParam(createParamCentered<LEDBezel>(mm2px(Vec(10.16f, 92.94f - 2.54f)), module, SemitoneSequencer::RUNNING_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(10.16f, 92.94f - 2.54f)), module, SemitoneSequencer::RUNNING_LIGHT));
        addParam(createParamCentered<TAR::Components::LightKnob>(mm2px(Vec(35.56f, 92.94f - 2.54)), module, SemitoneSequencer::CLOCK_PARAM));
        addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(35.56f, 92.94f - 2.54)), module, SemitoneSequencer::GATE_LIGHT));
        addParam(createParamCentered<TAR::Components::HorizontalSwitchThree>(mm2px(Vec(20.32f, 92.94f - 2.54f)), module, SemitoneSequencer::MODE_SWITCH_PARAM));
        addParam(createParamCentered<LEDBezel>(mm2px(Vec(60.96f, 92.94f - 2.54f)), module, SemitoneSequencer::RESET_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(60.96f, 92.94f - 2.54f)), module, SemitoneSequencer::RESET_LIGHT));

        /**addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(21.59, 21.82f + 13.16)), module, SemitoneSequencer::STEP1_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(21.59, 21.82f + 13.16)), module, SemitoneSequencer::STEP1_ACTIVE_LIGHT));
        addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(21.59, 42.14f + 13.16)), module, SemitoneSequencer::STEP2_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(21.59, 42.14f + 13.16)), module, SemitoneSequencer::STEP2_ACTIVE_LIGHT));
        addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(21.59, 62.46f + 13.16)), module, SemitoneSequencer::STEP3_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(21.59, 62.46f + 13.16)), module, SemitoneSequencer::STEP3_ACTIVE_LIGHT));
        addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(21.59, 82.78f + 13.16)), module, SemitoneSequencer::STEP4_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(21.59, 82.78f + 13.16)), module, SemitoneSequencer::STEP4_ACTIVE_LIGHT));
        addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(49.425, 21.82f + 13.16)), module, SemitoneSequencer::STEP5_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(49.425, 21.82f + 13.16)), module, SemitoneSequencer::STEP5_ACTIVE_LIGHT));
        addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(49.425, 42.14f + 13.16)), module, SemitoneSequencer::STEP6_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(49.425, 42.14f + 13.16)), module, SemitoneSequencer::STEP6_ACTIVE_LIGHT));
        addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(49.425, 62.46f + 13.16)), module, SemitoneSequencer::STEP7_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(49.425, 62.46f + 13.16)), module, SemitoneSequencer::STEP7_ACTIVE_LIGHT));
        addParam(createParamCentered<TAR::Components::NonMomentaryLEDBezel>(mm2px(Vec(49.425, 82.78f + 13.16)), module, SemitoneSequencer::STEP8_ACTIVE_PARAM));
        addChild(createLightCentered<LEDBezelLight<WhiteLight>>(mm2px(Vec(49.425, 82.78f + 13.16)), module, SemitoneSequencer::STEP8_ACTIVE_LIGHT));*/

        addInput(createInputCentered<TAR::Components::Port1>(mm2px(Vec(12.7f, 103.1f)), module, SemitoneSequencer::CLOCK_INPUT));
        addInput(createInputCentered<TAR::Components::Port1>(mm2px(Vec(27.94f, 103.1f)), module, SemitoneSequencer::RESET_INPUT));
        addOutput(createOutputCentered<TAR::Components::Port1>(mm2px(Vec(43.18f, 103.1f)), module, SemitoneSequencer::GATE_OUTPUT));
        addOutput(createOutputCentered<TAR::Components::Port1>(mm2px(Vec(58.42, 103.1f)), module, SemitoneSequencer::CV_OUTPUT));

        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(5.08f, 21.82f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT1_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(5.08f, 42.14f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT2_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(5.08f, 62.46f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT3_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(5.08f, 82.78f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT4_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(66.04f, 21.82f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT5_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(66.04f, 42.14f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT6_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(66.04f, 62.46f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT7_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(66.04f, 82.78f - 5.08f)), module, SemitoneSequencer::CURRENT_LIGHT8_LIGHT));

        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(20.32f, 9.0f)), module, SemitoneSequencer::CURRENT_MEASURE1_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(30.48f, 9.0f)), module, SemitoneSequencer::CURRENT_MEASURE2_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(40.64f, 9.0f)), module, SemitoneSequencer::CURRENT_MEASURE3_LIGHT));
        addChild(createLightCentered<MediumLight<TAR::Components::BlueWhiteLight>>(mm2px(Vec(50.8f, 9.0f)), module, SemitoneSequencer::CURRENT_MEASURE4_LIGHT));

        SequencerWidget *sequencerWidget = createWidget<SequencerWidget>(
            mm2px(rack::math::Vec(81.28f, 26.06f)));
        sequencerWidget->box.size = mm2px(Vec(50.8f, 18.0f));
        sequencerWidget->setWidget((module) ? module : NULL);
        addChild(sequencerWidget);
    }
};

Model *modelSemitoneSequencer = createModel<SemitoneSequencer, SemitoneSequencerWidget>("SemitoneSequencer");
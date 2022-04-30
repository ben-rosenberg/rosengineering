#ifndef SEQUENCER_WIDGET_H
#define SEQUENCER_WIDGET_H

#include "../plugin.hpp"
#include "RosEngineering.hpp"
#include "../SemitoneSequencer.hpp"

struct SemitoneSequencer;

struct SequencerWidget : LedDisplay
{
    LedDisplayChoice *numStepsPerMeasureChoice;
    LedDisplaySeparator *numStepsPerMeasureSeparator;
    LedDisplayChoice *numMeasuresChoice;
    LedDisplaySeparator *numMeasuresSeparator;
    LedDisplayChoice *chordModeChoice;
    LedDisplaySeparator *chordModeSeparator;
    LedDisplayChoice *quantizationModeChoice;
    LedDisplaySeparator *quantizationModeSeparator;
    LedDisplayChoice *detuneModeChoice;
    LedDisplaySeparator *detuneModeSeparator;

    void setWidget(SemitoneSequencer *semitoneSequencer);
};

#endif // SEQUENCER_WIDGET_H
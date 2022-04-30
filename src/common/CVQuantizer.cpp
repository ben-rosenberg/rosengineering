#include "CVQuantizer.hpp"
#include "REMath.hpp"

CVGuidedQuantizer::CVGuidedQuantizer()
    : m_UnquantizedSemitoneInCV(0.0f), m_InputOctaveNumber(0.0f), m_NumGuideNotes(0)
{}

void CVGuidedQuantizer::SetAllowedNotes(float guideNotesCVArray[], const int numGuideNotes)
{
    m_NumGuideNotes = numGuideNotes;

    for (int i = 0; i < numGuideNotes; ++i) {
        m_GuideNotesInCV[i] = std::fabs(guideNotesCVArray[i]) - std::trunc(guideNotesCVArray[i]); // I changed this! idk if it/s right. moved paren from end to after the fabs
    }
}

void CVGuidedQuantizer::SetAllowedNotesDefault(uint8_t scaleParamValue, uint8_t noteParamValue)
{
    switch(scaleParamValue) {
        case 0: 
    }
}
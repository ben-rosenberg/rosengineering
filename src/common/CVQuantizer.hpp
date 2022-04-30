#pragma once


class CVGuidedQuantizer
{
public:
    CVGuidedQuantizer();

    void SetAllowedNotes(float guideNotesCVArray[], const int numGuideNotes);
    void SetAllowedNotesDefault(uint8_t scaleParamValue, uint8_t noteParamValue);

    float quantize(float inputPitchCV)
	{
		m_InputOctaveNumber = std::floor(inputPitchCV);
		m_UnquantizedSemitoneInCV = inputPitchCV - m_InputOctaveNumber;

		float currentClosestSemitoneInCV = m_GuideNotesInCV[0];
		float currentSmallestDistanceToAllowedNote = std::fabs(m_UnquantizedSemitoneInCV - m_GuideNotesInCV[0]);
		for (int i = 1; i < m_NumGuideNotes; ++i)
		{
			if (std::fabs(m_UnquantizedSemitoneInCV - m_GuideNotesInCV[i]) < currentSmallestDistanceToAllowedNote)
			{
				currentClosestSemitoneInCV = m_GuideNotesInCV[i];
				currentSmallestDistanceToAllowedNote = std::fabs(m_UnquantizedSemitoneInCV - m_GuideNotesInCV[i]);
			}
		}
            
        return currentClosestSemitoneInCV + m_InputOctaveNumber;
	}

private:
	float m_GuideNotesInCV[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	float m_UnquantizedSemitoneInCV;
	float m_InputOctaveNumber;

	uint8_t m_NumGuideNotes;

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

    
    /* float minor[7] = { 0.0f, 0.166667, 0.25f, 0.416667f, 0.583333f, 0.6666667f, 0.8333333f};
	float major[7] = { 0.0f, 0.166667, 0.333333f, 0.416667f, 0.583333f, 0.75f, 0.916667};
	float minorTriad[3] = {0.0f, 0.25f, 0.583333f};
	float majorTriad[3] = {0.0f, 0.333333f, 0.5833333f}; */
};
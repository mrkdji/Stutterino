/*
  ==============================================================================

    Parameters.h
    Created: 2 May 2023 7:07:07pm
    Author:  mrkdji

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const juce::StringArray noteLengthUnitChoices = juce::StringArray("Seconds", "Beats");


namespace IDs {
    const char* const lengthInSecondsOrBeats = "lengthInSecondsOrBeats";
    const char* const noteLengthSeconds = "noteLengthSeconds";
    const char* const noteLengthBeats = "noteLengthBeats";
    const char* const divisions = "divisions";
    const char* const divisionsLengthPercentage = "divisionsLengthPercentage";
    const char* const pitchShiftStep = "pitchShiftStep";
    const char* const skew = "skew";
}

enum class NoteLengthUnit {
    SECONDS,
    BEATS
};

const int defaultNoteLengthChoice = 7;

const juce::StringArray NoteLengthChoices{
    "1/64",
    "1/32",
    "1/16",
    "1/8",
    "1/4",
    "1/3",
    "1/2",
    "1",
    "2",
    "3",
    "4"
};

constexpr float NoteLengths[] {
    1.0f / 64.0f,
    1.0f / 32.0f,
    1.0f / 16.0f,
    1.0f / 8.0f,
    1.0f / 4.0f,
    1.0f / 3.0f,
    1.0f / 2.0f,
    1.0f,
    2.0f,
    3.0f,
    4.0f
};

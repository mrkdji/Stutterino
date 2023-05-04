/*
  ==============================================================================

    VisualizerData.h
    Created: 4 May 2023 10:46:45am
    Author:  mrkdji

  ==============================================================================
*/

#pragma once

#include <vector>

struct VisualizerData {

    VisualizerData(int divisions, float divisionsLengthPercentage, const std::vector<float>& noteStartTimes) : 
        divisions(divisions),
        divisionsLengthPercentage(divisionsLengthPercentage),
        noteStartTimes(noteStartTimes)
    {}

    int divisions;
    float divisionsLengthPercentage;
    const std::vector<float>& noteStartTimes;
};
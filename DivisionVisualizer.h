/*
  ==============================================================================

    DivisionVisualizer.h
    Created: 3 May 2023 12:03:47pm
    Author:  lirija

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
using DivisionVisualizerCallback = std::function<std::vector<float>()>;

class DivisionVisualizer  : public juce::Component
{
public:
    DivisionVisualizer(DivisionVisualizerCallback getPoints) :
        getPoints(getPoints) {}

    DivisionVisualizer();
    ~DivisionVisualizer() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    DivisionVisualizerCallback getPoints;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DivisionVisualizer)
};

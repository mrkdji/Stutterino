/*
  ==============================================================================

    DivisionVisualizer.h
    Created: 3 May 2023 12:03:47pm
    Author:  lirija

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MIDINoteRepeater;
//==============================================================================
/*
*/

class DivisionVisualizer  : public juce::Component
{
public:

    DivisionVisualizer(MIDINoteRepeater* repeater) :
        repeater(repeater)
    {}

    DivisionVisualizer();
    ~DivisionVisualizer() override;

    void paint (juce::Graphics&) override;
    void resized() override;


private:
    MIDINoteRepeater* repeater;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DivisionVisualizer)
};

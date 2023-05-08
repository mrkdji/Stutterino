/*
  ==============================================================================

    DivisionVisualizer.h
    Created: 3 May 2023 12:03:47pm
    Author:  mrkdji

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MIDINoteRepeater;
//==============================================================================
/*
*/

class DivisionVisualizer  : 
    public juce::Component, 
    public juce::Timer
{
public:

    static const int backgroundColourId = 1;
    static const int divisionsColourId = 2;

    DivisionVisualizer(MIDINoteRepeater* repeater) :
        repeater(repeater)
    {
        startTimer(50);
    }

    ~DivisionVisualizer() 
    {
        stopTimer();
    }

    virtual void timerCallback() override;

    void paint (juce::Graphics&) override;
    void resized() override;


private:
    MIDINoteRepeater* repeater;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DivisionVisualizer)
};

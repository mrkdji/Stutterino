/*
  ==============================================================================

    DivisionVisualizer.cpp
    Created: 3 May 2023 12:03:47pm
    Author:  mrkdji

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DivisionVisualizer.h"
#include "MIDINoteRepeater.h"
#include "VisualizerData.h"

//==============================================================================

void DivisionVisualizer::timerCallback()
{
    repaint();
}

void DivisionVisualizer::paint (juce::Graphics& g)
{
    g.fillAll ( getLookAndFeel().findColour(DivisionVisualizer::backgroundColourId) );

    g.setColour (juce::Colours::white);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour( getLookAndFeel().findColour(DivisionVisualizer::divisionsColourId) );

    constexpr int margin = 4;
    constexpr int spacing = 1;
    constexpr float cornerSize = 2.0f;
    
    auto bounds = getLocalBounds().reduced(margin);

    auto rect = juce::Rectangle<float>();
    rect.setY(bounds.getY());
    rect.setHeight(bounds.getHeight());

    VisualizerData data = repeater->getDataForVisualizer();

    for (int i = 0; i < data.divisions; i++)
    {
        rect.setX(bounds.getX() + data.noteStartTimes[i] * bounds.getWidth() );
        float next = (i == data.divisions - 1) ? 1.0f : data.noteStartTimes[i + 1];
        float w = bounds.getWidth() * (next - data.noteStartTimes[i]);
        rect.setWidth(w * data.divisionsLengthPercentage - spacing);
        g.fillRoundedRectangle(rect, cornerSize);
    }

}

void DivisionVisualizer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

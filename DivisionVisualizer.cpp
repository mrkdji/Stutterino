/*
  ==============================================================================

    DivisionVisualizer.cpp
    Created: 3 May 2023 12:03:47pm
    Author:  lirija

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DivisionVisualizer.h"

//==============================================================================
DivisionVisualizer::DivisionVisualizer()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

DivisionVisualizer::~DivisionVisualizer()
{
}

void DivisionVisualizer::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (juce::Colours::black);   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::green);

    std::vector<float> points = getPoints();

    int margin = 2;
    int spacing = 1;
    float cornerSize = 5.0f;
    int divisions = juce::jmax<int>(1, points.size());

    auto bounds = getLocalBounds().reduced(margin);

    auto rect = juce::Rectangle<float>(bounds.getX(), bounds.getY(), (bounds.getWidth() / divisions) - spacing, bounds.getHeight());
    for (int i = 0; i < divisions; i++)
    {
        rect.setX(bounds.getX() + points[i] * bounds.getWidth() );
        float w = (i == divisions - 1) ? (1.0f - points.back()) : points[i + 1] - points[i];
        rect.setWidth(w * bounds.getWidth() - spacing);
        g.fillRoundedRectangle(rect, cornerSize);
    }

}

void DivisionVisualizer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

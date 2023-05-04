/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Parameters.h"



//==============================================================================
MIDINoteRepeaterAudioProcessorEditor::MIDINoteRepeaterAudioProcessorEditor (MIDINoteRepeaterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    divisionVisualizer(&audioProcessor.repeater),
    noteLengthUnitAttachment(p.apvts, IDs::lengthInSecondsOrBeats, noteLengthUnitCombobox),
    noteLengthInSecondsAttachment(p.apvts, IDs::noteLengthSeconds, noteLengthInSecondsSlider),
    noteLengthInBeatsAttachment(p.apvts, IDs::noteLengthBeats, noteLengthInBeatsComboBox),
    divisionsAttachment(p.apvts, IDs::divisions, divisionsSlider),
    divisionLengthPercentageAttachment(p.apvts, IDs::divisionsLengthPercentage, divisionLengthPercentageSlider),
    pitchShiftStepAttachment(p.apvts, IDs::pitchShiftStep, pitchShiftStepSlider),
    skewAttachment(p.apvts, IDs::skew, skewSlider)
{
    setResizable(true, true);
    setResizeLimits(400, 300, 1200, 900);
    setSize (400, 300);

    for (auto c : getComponents())
    {
        addAndMakeVisible(c);
    }

    noteLengthUnitCombobox.addItemList(noteLengthUnitChoices, 1);
    noteLengthInBeatsComboBox.addItemList(NoteLengthChoices, 1);

    noteLengthUnitCombobox.addListener(this);

    for (auto id : { IDs::divisions, IDs::skew, IDs::divisionsLengthPercentage })
        audioProcessor.apvts.getParameter(id)->addListener(this);

}

MIDINoteRepeaterAudioProcessorEditor::~MIDINoteRepeaterAudioProcessorEditor()
{
    for (auto id : { IDs::divisions, IDs::skew, IDs::divisionsLengthPercentage })
        audioProcessor.apvts.getParameter(id)->removeListener(this);
}

void MIDINoteRepeaterAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
    const juce::MessageManagerLock mmLock;
    divisionVisualizer.repaint();
}

void MIDINoteRepeaterAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) 
{
    NoteLengthUnit unit = static_cast<NoteLengthUnit>(comboBoxThatHasChanged->getSelectedItemIndex());

    noteLengthInSecondsSlider.setVisible(unit == NoteLengthUnit::SECONDS);
    noteLengthInBeatsComboBox.setVisible(unit == NoteLengthUnit::BEATS);
}

std::vector<juce::Component*> MIDINoteRepeaterAudioProcessorEditor::getComponents()
{
    return {
    &noteLengthUnitCombobox,
    &noteLengthInSecondsSlider,
    &noteLengthInBeatsComboBox,
    &divisionVisualizer,
    &divisionsSlider,
    &divisionLengthPercentageSlider,
    &pitchShiftStepSlider,
    &skewSlider
    };
}


//==============================================================================
void MIDINoteRepeaterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MIDINoteRepeaterAudioProcessorEditor::resized()
{
    const int noteLengthSectionHeight = 30;
    const int divisionVisualizerHeight = 50;
    const int margin = 2;

    juce::Rectangle bounds = getLocalBounds();
    juce::Rectangle noteLengthSection = bounds.removeFromTop(noteLengthSectionHeight).reduced(margin);
    juce::Rectangle divisionVisualizerSection = bounds.removeFromTop(divisionVisualizerHeight).reduced(margin);
    juce::Rectangle divisionSection = bounds.removeFromTop(bounds.getHeight() * 0.5f).reduced(margin);
    juce::Rectangle fxSection = bounds.reduced(margin);


    noteLengthUnitCombobox.setBounds(noteLengthSection.removeFromLeft(getWidth() * 0.4f).reduced(margin));
    noteLengthInSecondsSlider.setBounds(noteLengthSection.reduced(margin));
    noteLengthInBeatsComboBox.setBounds(noteLengthSection.reduced(margin));

    divisionVisualizer.setBounds(divisionVisualizerSection);

    divisionsSlider.setBounds(divisionSection.removeFromLeft(getWidth() * 0.5f).reduced(margin));
    divisionLengthPercentageSlider.setBounds(divisionSection.reduced(margin));

    pitchShiftStepSlider.setBounds(fxSection.removeFromLeft(getWidth() * 0.5f).reduced(margin));
    skewSlider.setBounds(fxSection.reduced(margin));

}

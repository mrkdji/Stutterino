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
    noteLengthInSecondsAttachment(p.apvts, IDs::noteLengthSeconds, noteLengthInSecondsSlider),
    divisionsAttachment(p.apvts, IDs::divisions, divisionsSlider),
    divisionLengthPercentageAttachment(p.apvts, IDs::divisionsLengthPercentage, divisionLengthPercentageSlider),
    pitchShiftStepAttachment(p.apvts, IDs::pitchShiftStep, pitchShiftStepSlider),
    skewAttachment(p.apvts, IDs::skew, skewSlider)
{
    // attach labels and tooltips to components;

    setResizable(true, true);
    setResizeLimits(400, 300, 1200, 900);
    setSize (400, 300);

    pluginName.setText("Stutterino", juce::NotificationType::dontSendNotification);
    pluginName.setJustificationType(juce::Justification::centred);

    for (auto c : getComponents())
    {
        addAndMakeVisible(c);
    }

    noteLengthUnitCombobox.addItemList(noteLengthUnitChoices, 1);
    noteLengthInBeatsComboBox.addItemList(NoteLengthChoices, 1);
    noteLengthUnitAttachment = std::make_unique<ComboBoxAttachment>(p.apvts, IDs::lengthInSecondsOrBeats, noteLengthUnitCombobox);
    noteLengthInBeatsAttachment = std::make_unique<ComboBoxAttachment>(p.apvts, IDs::noteLengthBeats, noteLengthInBeatsComboBox);
    noteLengthUnitCombobox.addListener(this);

    comboBoxChanged(&noteLengthUnitCombobox);
}

MIDINoteRepeaterAudioProcessorEditor::~MIDINoteRepeaterAudioProcessorEditor()
{
    noteLengthUnitCombobox.removeListener(this);
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
    &pluginName,
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
}

void MIDINoteRepeaterAudioProcessorEditor::resized()
{
    const int pluginNameHeight = 30;
    const int noteLengthSectionHeight = 30;
    const int divisionVisualizerHeight = 60;
    const int margin = 2;

    juce::Rectangle bounds = getLocalBounds();
    juce::Rectangle pluginNameSection = bounds.removeFromTop(pluginNameHeight);
    juce::Rectangle noteLengthSection = bounds.removeFromTop(noteLengthSectionHeight).reduced(margin);
    juce::Rectangle divisionVisualizerSection = bounds.removeFromTop(divisionVisualizerHeight).reduced(margin);
    juce::Rectangle divisionSection = bounds.removeFromTop(bounds.getHeight() * 0.5f).reduced(margin);
    juce::Rectangle fxSection = bounds.reduced(margin);

    pluginName.setBounds(pluginNameSection);

    noteLengthUnitCombobox.setBounds(noteLengthSection.removeFromLeft(getWidth() * 0.4f).reduced(margin));
    noteLengthInSecondsSlider.setBounds(noteLengthSection.reduced(margin));
    noteLengthInBeatsComboBox.setBounds(noteLengthSection.reduced(margin));

    divisionVisualizer.setBounds(divisionVisualizerSection);

    divisionsSlider.setBounds(divisionSection.removeFromLeft(getWidth() * 0.5f).reduced(margin));
    divisionLengthPercentageSlider.setBounds(divisionSection.reduced(margin));

    pitchShiftStepSlider.setBounds(fxSection.removeFromLeft(getWidth() * 0.5f).reduced(margin));
    skewSlider.setBounds(fxSection.reduced(margin));

}

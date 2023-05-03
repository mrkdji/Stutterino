/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MIDINoteRepeaterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MIDINoteRepeaterAudioProcessorEditor (MIDINoteRepeaterAudioProcessor&);
    ~MIDINoteRepeaterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MIDINoteRepeaterAudioProcessor& audioProcessor;


    juce::ComboBox noteLengthUnitCombobox;
    juce::Slider noteLengthInSecondsSlider;
    juce::ComboBox noteLengthInBeatsComboBox;
    juce::Slider divisionsSlider;
    juce::Slider divisionLengthPercentageSlider;
    juce::Slider pitchShiftStepSlider;
    juce::Slider skewSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    ComboBoxAttachment noteLengthUnitAttachment;
    SliderAttachment noteLengthInSecondsAttachment;
    ComboBoxAttachment noteLengthInBeatsAttachment;
    SliderAttachment divisionsAttachment;
    SliderAttachment divisionLengthPercentageAttachment;
    SliderAttachment pitchShiftStepAttachment;
    SliderAttachment skewAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDINoteRepeaterAudioProcessorEditor)
};

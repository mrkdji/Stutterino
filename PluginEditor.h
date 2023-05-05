/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DivisionVisualizer.h"

//==============================================================================
/**
*/
class MIDINoteRepeaterAudioProcessorEditor :
    public juce::AudioProcessorEditor,
    public juce::ComboBox::Listener
{
public:
    MIDINoteRepeaterAudioProcessorEditor(MIDINoteRepeaterAudioProcessor&);
    ~MIDINoteRepeaterAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    virtual void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:

    using JuceSliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    class Slider : public juce::Component {
    public:
        Slider() {
            description.setJustificationType(juce::Justification::centred);
            addAndMakeVisible(slider);
            addAndMakeVisible(description);
        }
        
        void resized() override
        {
            constexpr int margin = 2;

            auto bounds = getLocalBounds();
            description.setBounds(bounds.removeFromTop(description.getFont().getHeight() + margin));
            slider.setBounds(bounds);
        }

        juce::Slider slider;
        juce::Label description;
    };

    class SliderAttachment : public JuceSliderAttachment 
    {
    public:
        SliderAttachment(juce::AudioProcessorValueTreeState& stateToUse,
            const juce::String& parameterID,
            Slider& slider) : 
            JuceSliderAttachment(stateToUse,
                parameterID, slider.slider)
        {
            slider.description.setText( stateToUse.getParameter(parameterID)->name, juce::NotificationType::dontSendNotification );
        };
    };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MIDINoteRepeaterAudioProcessor& audioProcessor;

    std::vector<juce::Component*> getComponents();

    juce::Label pluginName;

    DivisionVisualizer divisionVisualizer;

    juce::ComboBox noteLengthUnitCombobox;
    juce::ComboBox noteLengthInBeatsComboBox;
    std::unique_ptr<ComboBoxAttachment> noteLengthUnitAttachment;
    std::unique_ptr<ComboBoxAttachment> noteLengthInBeatsAttachment;

    juce::Slider noteLengthInSecondsSlider;
    JuceSliderAttachment noteLengthInSecondsAttachment;

    Slider divisionsSlider;
    Slider divisionLengthPercentageSlider;
    Slider pitchShiftStepSlider;
    Slider skewSlider;

    SliderAttachment divisionsAttachment;
    SliderAttachment divisionLengthPercentageAttachment;
    SliderAttachment pitchShiftStepAttachment;
    SliderAttachment skewAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDINoteRepeaterAudioProcessorEditor)
};

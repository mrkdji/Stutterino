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


class Styles : public juce::LookAndFeel_V4 
{
public:
    enum Colors {
        LIGHTEST,
        LIGHT,
        DARK,
        DARKEST
    };


    Styles();

    juce::Path getTickShape(float height) override
    {
        juce::Path path;

        float radius = height * 0.5f;        
        path.addEllipse(-radius, -radius, height, height);

        juce::PathStrokeType pst( 0.1f );
        pst.createStrokedPath(path, path);

        return path;
    }

    
    virtual juce::Font getComboBoxFont(juce::ComboBox& box) override 
    { return defaultFont; }

    virtual juce::Font getSliderPopupFont(juce::Slider& s) override
    { return defaultFont; }

    const int defaultFontHeight = 20;
    const int titleFontHeight = 40;
    const juce::Font defaultFont = juce::Font(defaultFontHeight);
    const juce::Font titleFont = juce::Font(titleFontHeight);
    const int outerRectMargin = 5;
    const int outerRectLineThickness = 4;

    std::vector<juce::Colour> colors;
};


inline Styles* getStyles(juce::Component* c) {
    return static_cast<Styles*>( &c->getLookAndFeel() );
}


class MIDINoteRepeaterAudioProcessorEditor :
    public juce::AudioProcessorEditor
{
public:
    MIDINoteRepeaterAudioProcessorEditor(MIDINoteRepeaterAudioProcessor&);
    ~MIDINoteRepeaterAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Styles styles;

    using JuceSliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    class Slider : public juce::Component {
    public:
        Slider() {
            description.setJustificationType(juce::Justification::centred);
            description.setFont( getStyles(this)->defaultFont );
            addAndMakeVisible(slider);
            addAndMakeVisible(description);

            slider.setTextBoxIsEditable(false);
            slider.hideTextBox(false);
        }
        
        void resized() override
        {
            constexpr int margin = 2;

            auto bounds = getLocalBounds();
            description.setBounds(bounds.removeFromTop( getStyles(this)->defaultFontHeight + margin ));
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


    juce::Label noteDurationLabel;
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

    juce::TooltipWindow tooltipWindow;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDINoteRepeaterAudioProcessorEditor)
};

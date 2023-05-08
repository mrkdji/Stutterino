/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Parameters.h"

// palette colours should be ordered from light to dark
static const std::vector<juce::String> T_LOLLIPOP_PALETTE = {
    "#e6f2ef",
    "#f783b0",
    "#3f6d9e",
    "#151640"
};

static const std::vector<juce::String> MINTY_FRESH_PALETTE = {
    "#fbffe0",
    "#95c798",
    "#856d52",
    "#40332f"
};

Styles::Styles() 
{
    const std::vector<juce::String>& palette = T_LOLLIPOP_PALETTE;

    for (int i = 0; i < palette.size(); i++)
    {
        colors.push_back( juce::Colour::fromString(palette[i]).withAlpha(1.0f) );
    }

    setColour(juce::Label::textColourId, colors[Colors::LIGHTEST]);
    setColour(juce::ResizableWindow::backgroundColourId, colors[Colors::DARK]);

    setColour(juce::Slider::thumbColourId, colors[Colors::LIGHT]);
    setColour(juce::Slider::trackColourId, colors[Colors::DARKEST]);
    setColour(juce::Slider::backgroundColourId, colors[Colors::DARKEST]);

    setColour(juce::Slider::textBoxTextColourId, colors[Colors::LIGHTEST]);
    setColour(juce::Slider::textBoxBackgroundColourId, colors[Colors::DARKEST]);
    setColour(juce::Slider::textBoxOutlineColourId, colors[Colors::LIGHTEST]);

    setColour(DivisionVisualizer::backgroundColourId, colors[Colors::DARKEST]);
    setColour(DivisionVisualizer::divisionsColourId, colors[Colors::LIGHT]);

    setColour(juce::ComboBox::backgroundColourId, colors[Colors::DARK]);
    setColour(juce::ComboBox::outlineColourId, colors[Colors::LIGHTEST]);
    setColour(juce::ComboBox::arrowColourId, colors[Colors::LIGHTEST]);
    setColour(juce::PopupMenu::backgroundColourId, colors[Colors::DARK]);
    setColour(juce::PopupMenu::textColourId, colors[Colors::LIGHTEST]);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, colors[Colors::DARKEST]);
    setColour(juce::PopupMenu::highlightedTextColourId, colors[Colors::LIGHT]);

    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::alatamodded_otf, BinaryData::alatamodded_otfSize);
    setDefaultSansSerifTypeface(typeface);

    setDefaultLookAndFeel(this);
}

//eventually change sliders into rotary, add a rounded rect as the plugin bounds.

//==============================================================================
MIDINoteRepeaterAudioProcessorEditor::MIDINoteRepeaterAudioProcessorEditor (MIDINoteRepeaterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    styles(),
    divisionVisualizer(&audioProcessor.repeater),
    noteLengthInSecondsAttachment(p.apvts, IDs::noteLengthSeconds, noteLengthInSecondsSlider),
    divisionsAttachment(p.apvts, IDs::divisions, divisionsSlider),
    divisionLengthPercentageAttachment(p.apvts, IDs::divisionsLengthPercentage, divisionLengthPercentageSlider),
    pitchShiftStepAttachment(p.apvts, IDs::pitchShiftStep, pitchShiftStepSlider),
    skewAttachment(p.apvts, IDs::skew, skewSlider)
{
    constexpr int pluginWidth = 400;
    constexpr int pluginHeight = 300;
    //setLookAndFeel(&styles);

    setResizable(true, true);
    
    setResizeLimits(pluginWidth, pluginHeight, pluginWidth * 3, pluginHeight * 3);
    setSize(pluginWidth, pluginHeight);

    getConstrainer()->setFixedAspectRatio(pluginWidth / (float)pluginHeight);

    noteDurationLabel.setText("Select Note Duration: ", juce::NotificationType::dontSendNotification);
    noteDurationLabel.setJustificationType(juce::Justification::centredLeft);
    noteDurationLabel.setFont(styles.defaultFont);
    noteDurationLabel.setTooltip("Length can be optionally measured in seconds or measures");

    noteLengthInSecondsSlider.setTextBoxIsEditable(false);

    pluginName.setText("Stutterino", juce::NotificationType::dontSendNotification);
    pluginName.setFont(styles.titleFont);
    pluginName.setJustificationType(juce::Justification::right);

    for (auto c : getComponents())
    {
        addAndMakeVisible(c);
    }

    noteLengthUnitCombobox.addItemList(noteLengthUnitChoices, 1);
    noteLengthInBeatsComboBox.addItemList(NoteLengthChoices, 1);
    noteLengthUnitAttachment = std::make_unique<ComboBoxAttachment>(p.apvts, IDs::lengthInSecondsOrBeats, noteLengthUnitCombobox);
    noteLengthInBeatsAttachment = std::make_unique<ComboBoxAttachment>(p.apvts, IDs::noteLengthBeats, noteLengthInBeatsComboBox);

    noteLengthUnitCombobox.onChange = [this]() {
        NoteLengthUnit unit = static_cast<NoteLengthUnit>(noteLengthUnitCombobox.getSelectedItemIndex());

        noteLengthInSecondsSlider.setVisible(unit == NoteLengthUnit::SECONDS);
        noteLengthInBeatsComboBox.setVisible(unit == NoteLengthUnit::BEATS);
    };
    noteLengthUnitCombobox.onChange();

}

MIDINoteRepeaterAudioProcessorEditor::~MIDINoteRepeaterAudioProcessorEditor()
{}

std::vector<juce::Component*> MIDINoteRepeaterAudioProcessorEditor::getComponents()
{
    return {
    &noteDurationLabel,
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

//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll(styles.colors[Styles::Colors::DARKEST]);
    g.setColour(styles.colors[Styles::Colors::DARK]);
    g.fillRoundedRectangle(getX(), getY(), getWidth(), getHeight(), 5.0f);

    g.setColour(styles.colors[Styles::Colors::LIGHTEST]);
    g.drawRoundedRectangle( getLocalBounds().reduced(styles.outerRectMargin).toFloat(), 4, styles.outerRectLineThickness);

}


void MIDINoteRepeaterAudioProcessorEditor::resized()
{
    const int margin = 2;
    const int noteLengthSectionHeight = 3 * margin + 3 * styles.defaultFontHeight;
    const int divisionVisualizerHeight = 60;
    const int titleHeight = 2 * margin + styles.titleFontHeight;

    juce::Rectangle bounds = getLocalBounds().reduced(styles.outerRectMargin + styles.outerRectLineThickness);

    juce::Rectangle pluginNameSection = juce::Rectangle(bounds).removeFromTop(titleHeight);
    bounds.removeFromTop(titleHeight * 0.5);

    juce::Rectangle noteLengthSection = bounds.removeFromTop(noteLengthSectionHeight).reduced(margin);
    juce::Rectangle divisionVisualizerSection = bounds.removeFromTop(divisionVisualizerHeight).reduced(margin);
    juce::Rectangle divisionSection = bounds.removeFromTop(bounds.getHeight() * 0.5f).reduced(margin);
    juce::Rectangle fxSection = bounds.reduced(margin);

    pluginName.setBounds(pluginNameSection);

    noteDurationLabel.setBounds(noteLengthSection.removeFromTop( 2 * margin + styles.defaultFontHeight ) );
    noteLengthUnitCombobox.setBounds(noteLengthSection.removeFromLeft(getWidth() * 0.4f).reduced(margin));
    noteLengthInSecondsSlider.setBounds(noteLengthSection.reduced(margin));
    noteLengthInBeatsComboBox.setBounds(noteLengthSection.reduced(margin));

    divisionVisualizer.setBounds(divisionVisualizerSection);

    divisionsSlider.setBounds(divisionSection.removeFromLeft(getWidth() * 0.5f).reduced(margin));
    divisionLengthPercentageSlider.setBounds(divisionSection.reduced(margin));

    pitchShiftStepSlider.setBounds(fxSection.removeFromLeft(getWidth() * 0.5f).reduced(margin));
    skewSlider.setBounds(fxSection.reduced(margin));

}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

//==============================================================================
MIDINoteRepeaterAudioProcessor::MIDINoteRepeaterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    apvts(*this, nullptr, "PARAMETERS", createParametersLayout()),
    repeater(
        apvts,
        [this]() -> BPMAndTimeSignature {
            auto bpmOpt = getPlayHead()->getPosition()->getBpm();
            double bpm = bpmOpt ? *bpmOpt : 1.0;

            auto signatureOpt = getPlayHead()->getPosition()->getTimeSignature();
            juce::AudioPlayHead::TimeSignature signature = signatureOpt ? *signatureOpt : juce::AudioPlayHead::TimeSignature();

            return BPMAndTimeSignature(bpm, signature);
    })
#endif
{
}

MIDINoteRepeaterAudioProcessor::~MIDINoteRepeaterAudioProcessor()
{
}

//==============================================================================
const juce::String MIDINoteRepeaterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MIDINoteRepeaterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MIDINoteRepeaterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MIDINoteRepeaterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MIDINoteRepeaterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MIDINoteRepeaterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MIDINoteRepeaterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MIDINoteRepeaterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MIDINoteRepeaterAudioProcessor::getProgramName (int index)
{
    return {};
}

void MIDINoteRepeaterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MIDINoteRepeaterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    repeater.prepareToPlay(sampleRate, samplesPerBlock);
}

void MIDINoteRepeaterAudioProcessor::releaseResources()
{
    repeater.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MIDINoteRepeaterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void MIDINoteRepeaterAudioProcessor::buttonClicked(juce::Button* btn)
{

}

void MIDINoteRepeaterAudioProcessor::panic(juce::MidiBuffer& midiMessages)
{
    midiMessages.clear();
    for (int channel = 1; channel <= 16; channel++)
        for (int noteNumber = 0; noteNumber < 128; noteNumber++)
            midiMessages.addEvent(juce::MidiMessage::noteOff(channel, noteNumber), 0);
}


void MIDINoteRepeaterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (getPlayHead())
    {
        PlayHeadState currentState = getPlayHead()->getPosition()->getIsPlaying() ? PlayHeadState::PLAYING : PlayHeadState::PAUSED;

        if (currentState != playHeadState)
        {
            playHeadState = currentState;
            switch (playHeadState)
            {
                case PlayHeadState::PLAYING:
                {
                    juce::Logger::writeToLog("State set to Playing");
                    repeater.onPlayHeadStateChanged();
                    break;
                }

                case PlayHeadState::PAUSED:
                {
                    juce::Logger::writeToLog("State set to Paused");
                    repeater.onPlayHeadStateChanged();
                    panic(midiMessages);
                    break;
                }
            }
        }
    }

    repeater.process(midiMessages);
}

//==============================================================================
bool MIDINoteRepeaterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MIDINoteRepeaterAudioProcessor::createEditor()
{
    return new MIDINoteRepeaterAudioProcessorEditor (*this);
}

//==============================================================================

void MIDINoteRepeaterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MIDINoteRepeaterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState( juce::ValueTree::fromXml(*xmlState) );

}

juce::AudioProcessorValueTreeState::ParameterLayout MIDINoteRepeaterAudioProcessor::createParametersLayout()
{
    auto params = juce::AudioProcessorValueTreeState::ParameterLayout();

    params.add(
        std::make_unique<juce::AudioParameterChoice>(
            IDs::lengthInSecondsOrBeats,
            "Length In Seconds Or Beats",
            noteLengthUnitChoices,
            0
    ));

    params.add(
        std::make_unique<juce::AudioParameterFloat>(
            IDs::noteLengthSeconds,
            "Note Length",
            juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f),
            1.0f
    ));

    params.add(
        std::make_unique<juce::AudioParameterChoice>(
            IDs::noteLengthBeats,
            "Note Length",
            NoteLengthChoices,
            defaultNoteLengthChoice
    ));

    params.add(
        std::make_unique<juce::AudioParameterInt>(
            IDs::divisions,
            "Divisions",
            1,
            50,
            1
    ));

    params.add(
        std::make_unique<juce::AudioParameterFloat>(
            IDs::divisionsLengthPercentage,
            "Divisions Length Percentage",
            juce::NormalisableRange<float>(0.01f, 1.0f, 0.01f),
            1.0f
    ));

    params.add(
        std::make_unique<juce::AudioParameterInt>(
            IDs::pitchShiftStep,
            "Pitch Shift Step",
            -12,
            12,
            0
            ));

    params.add(
        std::make_unique<juce::AudioParameterFloat>(
            IDs::skew,
            "Skew",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
            0.0f
    ));

    return params;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MIDINoteRepeaterAudioProcessor();
}

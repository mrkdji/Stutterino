/*
  ==============================================================================

    MIDINoteRepeater.h
    Created: 27 Apr 2023 11:48:20am
    Author:  mrkdji

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class juce::MidiBuffer;

inline constexpr double MAX_DELAY_IN_SECONDS = 5.0;

class MIDINoteRepeater {

public:
    MIDINoteRepeater(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts) {}


    void onPlayHeadPause()
    {
        currentBlockIndex = 0;
        for (juce::MidiBuffer& midiBuffer : eventsQueue)
            midiBuffer.clear();
    }


    void releaseResources() 
    {
        juce::Logger::writeToLog("Releasing Resources");
    }


    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void process(juce::MidiBuffer& midiMessages);

    void getTimeStampForDelayedEvent(int delayedEventTimestamp);




    inline void increaseCurrentBlockIndex() { currentBlockIndex = (currentBlockIndex + 1) % eventsQueue.size(); }

private:
    int noteOnTimestamp;
    int noteOffTimestamp;
    int targetBlockIndex;
    int actualTimestamp;

    int currentBlockIndex = 0;
    std::vector<juce::MidiBuffer> eventsQueue;

    double sampleRate = -1.0;
    unsigned int samplesPerBlock = 0;

    juce::AudioProcessorValueTreeState& apvts;
};
/*
  ==============================================================================

    MIDINoteRepeater.h
    Created: 27 Apr 2023 11:48:20am
    Author:  mrkdji

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VisualizerData.h"

class juce::MidiBuffer;

struct BPMAndTimeSignature {
    BPMAndTimeSignature(double bpm, juce::AudioPlayHead::TimeSignature timeSignature) :
        bpm(bpm),
        timeSignature(timeSignature)
    {}

    double bpm;
    juce::AudioPlayHead::TimeSignature timeSignature;
};

class MIDINoteRepeater {

public:
    MIDINoteRepeater(juce::AudioProcessorValueTreeState& apvts, std::function<BPMAndTimeSignature()> getBPMAndTimeSignature) :
        apvts(apvts),
        cachedNoteStartTimes(50, 0.0f),
        getBPMAndTimeSignature(getBPMAndTimeSignature)
    {}


    void onPlayHeadStateChanged()
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

    VisualizerData getDataForVisualizer();

private:
    inline void increaseCurrentBlockIndex() { currentBlockIndex = (currentBlockIndex + 1) % eventsQueue.size(); }
    void queueMidiEvent(const juce::MidiMessage& message, int distanceInSamples);
    void updateCachedNotesStartTimes();

    int noteOnTimestamp;
    int noteOffTimestamp;
    int targetBlockIndex;
    int actualTimestamp;

    int currentBlockIndex = 0;
    std::vector<juce::MidiBuffer> eventsQueue;

    int cachedDivisions = 0;
    float cachedSkew = 0.0f;
    std::vector<float> cachedNoteStartTimes;

    double sampleRate = -1.0;
    unsigned int samplesPerBlock = 0;

    juce::AudioProcessorValueTreeState& apvts;

    std::function<BPMAndTimeSignature()> getBPMAndTimeSignature;
};
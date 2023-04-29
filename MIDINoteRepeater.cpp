/*
  ==============================================================================

    MIDINoteRepeater.cpp
    Created: 29 Apr 2023 4:46:13pm
    Author:  mrkdji

  ==============================================================================
*/

#include "MIDINoteRepeater.h"

void MIDINoteRepeater::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::Logger::writeToLog("Preparing to Play");

    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;

    currentBlockIndex = 0;
    eventsQueue.clear();
    eventsQueue.resize(static_cast<size_t>(MAX_DELAY_IN_SECONDS * sampleRate / samplesPerBlock));
}

void MIDINoteRepeater::getTimeStampForDelayedEvent(int delayedEventTimestamp)
{
    int delayInBlocks = (delayedEventTimestamp) / samplesPerBlock;
    targetBlockIndex = (currentBlockIndex + delayInBlocks) % eventsQueue.size();
    actualTimestamp = (delayedEventTimestamp) % samplesPerBlock;
}

void MIDINoteRepeater::process(juce::MidiBuffer& midiMessages)
{
    int numOfReps = apvts.getRawParameterValue("numOfReps")->load();
    if (numOfReps == 0)
        return;

    float delayInSeconds = apvts.getRawParameterValue("delayInSeconds")->load();
    float noteLength = apvts.getRawParameterValue("noteLength")->load();
    int pitchShiftStep = apvts.getRawParameterValue("pitchShiftStep")->load();
    float skewFactor = apvts.getRawParameterValue("skewFactor")->load();

    int skewSign = skewFactor > 0 ? 1 : skewFactor < 0 ? -1 : 0;
    int skewStrength = skewSign * skewFactor;

    // spostare roba nel .cpp
    // magari cambiare controlli e sceglierne di più comodi (note length, divisions, spacing, skew)
    // implementare skew non lineare!
    float skew = 0;

    unsigned int noteLengthInSamples = noteLength * sampleRate;
    unsigned int delayInSamples = delayInSeconds * sampleRate;

    for (const juce::MidiMessageMetadata& metadata : midiMessages)
    {
        const juce::MidiMessage& message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int noteNumber = message.getNoteNumber();

            for (float i = 0.0; i < numOfReps; i++)
            {
                float perc = i / (numOfReps - 1.0);

                if (skewSign)
                {
                    if (skewSign < 0)
                        perc = 1.0 - perc;

                    float res = perc;
                    for (int j = 1; j < skewStrength; j++)
                        res *= perc;

                    perc = res;

                    if (skewSign < 0)
                        perc = 1.0 - perc;
                }


                noteOnTimestamp = message.getTimeStamp() + perc * delayInSamples * numOfReps;
                //noteOnTimestamp = message.getTimeStamp() + (i + 1) * delayInSamples;

                noteOffTimestamp = noteOnTimestamp + noteLengthInSamples;

                getTimeStampForDelayedEvent(noteOnTimestamp);
                eventsQueue[targetBlockIndex].addEvent(
                    juce::MidiMessage::noteOn(message.getChannel(), noteNumber, juce::uint8(75)),
                    actualTimestamp
                );

                getTimeStampForDelayedEvent(noteOffTimestamp);
                eventsQueue[targetBlockIndex].addEvent(
                    juce::MidiMessage::noteOff(message.getChannel(), noteNumber, 0.0f),
                    actualTimestamp);

                if (pitchShiftStep)
                    noteNumber = juce::jlimit(0, 127, noteNumber + pitchShiftStep);
            }
        }
    }

    juce::MidiBuffer& currentBlock = eventsQueue[currentBlockIndex];

    if (!currentBlock.isEmpty())
        midiMessages.addEvents(currentBlock, 0, samplesPerBlock, 0);

    currentBlock.clear();
    increaseCurrentBlockIndex();
}
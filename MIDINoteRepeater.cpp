/*
  ==============================================================================

    MIDINoteRepeater.cpp
    Created: 29 Apr 2023 4:46:13pm
    Author:  mrkdji

  ==============================================================================
*/

#include "MIDINoteRepeater.h"
#include "Parameters.h"

VisualizerData MIDINoteRepeater::getDataForVisualizer()
{
    return VisualizerData(
        apvts.getRawParameterValue(IDs::divisions)->load(),
        apvts.getRawParameterValue(IDs::divisionsLengthPercentage)->load(),
        cachedNoteStartTimes
    );
}


void MIDINoteRepeater::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::Logger::writeToLog("Preparing to Play");

    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;

    currentBlockIndex = 0;
    eventsQueue.clear();

    float MAX_DELAY_IN_SECONDS = 5.0f;
    eventsQueue.resize(static_cast<size_t>(MAX_DELAY_IN_SECONDS * sampleRate / samplesPerBlock));

    updateCachedNotesStartTimes();
}

void MIDINoteRepeater::process(juce::MidiBuffer& midiMessages)
{

    int divisions = apvts.getRawParameterValue(IDs::divisions)->load();
    if (divisions == 1) return;

    NoteLengthUnit lengthInSecondsOrBeats = static_cast<NoteLengthUnit>(apvts.getRawParameterValue(IDs::lengthInSecondsOrBeats)->load());

    float noteLengthInSeconds;
    if (lengthInSecondsOrBeats == NoteLengthUnit::SECONDS) // length in seconds
    {
        noteLengthInSeconds = apvts.getRawParameterValue(IDs::noteLengthSeconds)->load();
    }
    else
    {
        int noteLengthChoice = apvts.getRawParameterValue(IDs::noteLengthBeats)->load();
        BPMAndTimeSignature info = getBPMAndTimeSignature();
        
        noteLengthInSeconds = NoteLengths[noteLengthChoice] * (60.0 / info.bpm) * info.timeSignature.numerator;        
    }

    float divisionsLengthPercentage = apvts.getRawParameterValue(IDs::divisionsLengthPercentage)->load();
    int pitchShiftStep = apvts.getRawParameterValue(IDs::pitchShiftStep)->load();

    float skew = apvts.getRawParameterValue(IDs::skew)->load();


    if ( (cachedDivisions != divisions) || (!juce::approximatelyEqual(skew, cachedSkew)) )
    {
        cachedDivisions = divisions;
        cachedSkew = skew;

        updateCachedNotesStartTimes();
    }

    unsigned int noteLengthInSamples = noteLengthInSeconds * sampleRate;

    float divisionLength = 0.0f;

    juce::MidiBuffer& currentBlock = eventsQueue[currentBlockIndex];

    for (const juce::MidiMessageMetadata& metadata : midiMessages)
    {
        const juce::MidiMessage& message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int noteNumber = message.getNoteNumber();

            // we re-add the original midi note to another buffer as we need to get rid of the original note off message.
            //currentBlock.addEvent(message, metadata.samplePosition);

            //queueMidiEvent(
            //    juce::MidiMessage::noteOff(message.getChannel(), noteNumber, juce::uint8(0)),
            //    metadata.samplePosition + cachedNoteStartTimes[1] * 
            //);

            float noteOnOffset = 0.0f;
            float nextNoteStartTime = 0.0f;

            for (int i = 0; i < cachedDivisions; i++)
            {
                if (pitchShiftStep)
                    noteNumber = juce::jlimit(0, 127, noteNumber + pitchShiftStep);

                noteOnOffset = cachedNoteStartTimes[i];

                nextNoteStartTime = (i == cachedDivisions - 1) ? 1.0f : cachedNoteStartTimes[i + 1];
                divisionLength = nextNoteStartTime - noteOnOffset;                

                noteOnTimestamp = message.getTimeStamp() + static_cast<int>(noteOnOffset * noteLengthInSamples);
                noteOffTimestamp = noteOnTimestamp + static_cast<int>(divisionLength * noteLengthInSamples * divisionsLengthPercentage) - 1;

               queueMidiEvent(
                    juce::MidiMessage::noteOn( message.getChannel(), noteNumber, message.getVelocity() ),
                    noteOnTimestamp
                );

                queueMidiEvent(
                    juce::MidiMessage::noteOff( message.getChannel(), noteNumber, juce::uint8(0) ),
                    noteOffTimestamp
                );

            }
        }
    }

    midiMessages.swapWith(currentBlock);

    currentBlock.clear();
    increaseCurrentBlockIndex();
}

void MIDINoteRepeater::updateCachedNotesStartTimes()
{
    juce::Logger::writeToLog("Updating cached notes start times");

    int skewSign = cachedSkew > 0 ? 1 : cachedSkew < 0 ? -1 : 0;
    float skewStrength = std::abs(cachedSkew);

    float step = 1.0f / cachedDivisions;
    float noteStart = 0.0f;

    for (int i = 1; i < cachedDivisions; i++)
    {
        noteStart = i * step;

        if (skewSign)
        {
            if (skewSign < 0)
                noteStart = 1 - noteStart;

            noteStart = juce::jmap(skewStrength, noteStart, noteStart * noteStart * (noteStart + step));

            if (skewSign < 0)
                noteStart = 1 - noteStart;
        }

        cachedNoteStartTimes[i] = noteStart;
    }
}

void MIDINoteRepeater::queueMidiEvent(const juce::MidiMessage& message, int distanceInSamples)
{
    int distanceInBlocks = (distanceInSamples) / samplesPerBlock;
    targetBlockIndex = (currentBlockIndex + distanceInBlocks) % eventsQueue.size();
    actualTimestamp = (distanceInSamples) % samplesPerBlock;

    eventsQueue[targetBlockIndex].addEvent(message, actualTimestamp);
}
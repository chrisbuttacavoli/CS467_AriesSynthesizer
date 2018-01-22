/*
  ==============================================================================

    SynthProcessor.cpp
    Created: 21 Jan 2018 9:55:11pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthProcessor.h"

//==============================================================================
SynthProcessor::SynthProcessor()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

SynthProcessor::~SynthProcessor()
{
}

const String SynthProcessor::getName() const {
	return String("My Synth Processor");
};

void SynthProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{

};

void SynthProcessor::releaseResources()
{

};
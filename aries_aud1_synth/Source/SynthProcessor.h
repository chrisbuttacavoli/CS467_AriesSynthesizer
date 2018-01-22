/*
  ==============================================================================

    SynthProcessor.h
    Created: 21 Jan 2018 9:55:11pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SynthProcessor : public AudioProcessor
{
public:
    SynthProcessor();
    ~SynthProcessor();

	const String getName() const override;
	
	void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

	void releaseResources() override;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthProcessor)
};

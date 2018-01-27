/*
==============================================================================

Oscillator.h
Created: 21 Jan 2018 3:47:03pm
Author:  Chris.Buttacavoli

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class OscillatorCombo : public Component,
	private ComboBox::Listener
{
public:
	OscillatorCombo();
	~OscillatorCombo();

	void paint(Graphics&) override;
	void resized() override;
	void comboBoxChanged(ComboBox*) override;

private:
	ComboBox cbOsc;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorCombo)
};
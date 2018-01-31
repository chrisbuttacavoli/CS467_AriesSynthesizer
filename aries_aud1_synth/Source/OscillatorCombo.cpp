/*
==============================================================================

Oscillator.cpp
Created: 21 Jan 2018 3:47:03pm
Author:  Chris.Buttacavoli

==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscillatorCombo.h"
#include "Oscillator.h"

//==============================================================================
OscillatorCombo::OscillatorCombo()
{
	// This class currently represent the choices/parameters for the Oscillators
	cbOsc.addItem("Sine", OscillatorType::sineWave);
	cbOsc.addItem("Saw", OscillatorType::sawWave);
	cbOsc.addItem("Square", OscillatorType::squareWave);
	cbOsc.addItem("Noise", OscillatorType::noiseWave);

	cbOsc.setSize(200, 200);
	cbOsc.setJustificationType(Justification::centred);
	addAndMakeVisible(cbOsc);
	cbOsc.addListener(this);
}

OscillatorCombo::~OscillatorCombo()
{
}


void OscillatorCombo::comboBoxChanged(ComboBox* cb)
{

}


void OscillatorCombo::paint(Graphics& g)
{
	/* This demo code just fills the component's background and
	draws some placeholder text to get you started.

	You should replace everything in this method with your own
	drawing code..
	*/

	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));   // clear the background

	g.setColour(Colours::grey);
	g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

	g.setColour(Colours::white);
	g.setFont(9.0f);
	g.drawText("OscillatorCombo", getLocalBounds(),
		Justification::centred, true);   // draw some placeholder text
}

void OscillatorCombo::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);
	cbOsc.setBounds(area.removeFromTop(20));
}
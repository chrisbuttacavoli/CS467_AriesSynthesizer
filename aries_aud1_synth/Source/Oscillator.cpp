/*
  ==============================================================================

    Oscillator.cpp
    Created: 21 Jan 2018 3:47:03pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Oscillator.h"
#include "Enums.h"

//==============================================================================
Oscillator::Oscillator()
{
	// This class currently represent the choices/parameters for the Oscillators
	cbOsc.addItem("Sine", sineWave);
	cbOsc.addItem("Saw", sawWave);
	cbOsc.addItem("Square", squareWave);
	
	cbOsc.setSize(200, 200);
	cbOsc.setJustificationType(Justification::centred);
	addAndMakeVisible(cbOsc);
	cbOsc.addListener(this);
}

Oscillator::~Oscillator()
{
}


void Oscillator::comboBoxChanged(ComboBox* cb)
{

}


void Oscillator::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (9.0f);
    g.drawText ("Oscillator", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void Oscillator::resized()
{
	Rectangle<int> area = getLocalBounds().reduced(40);
	cbOsc.setBounds(area.removeFromTop(20));
}

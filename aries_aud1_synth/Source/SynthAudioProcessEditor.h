/*
  ==============================================================================

    SynthAudioProcessEditor.h
    Created: 23 Jan 2018 7:22:31pm
    Author:  vdorn

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthProcessor.h"

//I dont think that we actually need this. I just implemented a rotary slider in the main editor class - Victoria

/*class SynthAudioProcessorEditor::RotarySlider : public Slider,
	private Timer
{
public:
	RotarySlider(AudioProcessorParameter& p)
		: Slider(p.getName(256)), param(p)
	{
		setRange(0.0, 1.0, 0.0);
		startTimerHz(30);
		updateSliderPos();
	}

	void valueChanged() override { param.setValueNotifyingHost((float)Slider::getValue()); }

	void timerCallback() override { updateSliderPos(); }

	void startedDragging() override { param.beginChangeGesture(); }
	void stoppedDragging() override { param.endChangeGesture(); }

	double getValueFromText(const String& text) override { return param.getValueForText(text); }
	String getTextFromValue(double value) override { return param.getText((float)value, 1024); }

	void updateSliderPos()
	{
		const float newValue = param.getValue();

		if (newValue != (float)Slider::getValue() && !isMouseButtonDown())
			Slider::setValue(newValue, NotificationType::dontSendNotification);
	}

	AudioProcessorParameter& param;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotarySlider)
};*/

//=================================================================================

class SynthAudioProcessorEditor : public AudioProcessorEditor,
	private Timer
{
public:	

	SynthAudioProcessorEditor(SynthProcessor& owner) : AudioProcessorEditor(owner),
		//midiKeyboard(owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),
		timecodeDisplayLabel(String()),
		gainLabel(String(), "Throughput level:")
	{
		// add some sliders..
		//addAndMakeVisible(gainSlider = new RotarySlider (*owner.gainParam)); - Previous code
		addAndMakeVisible(gainSlider);
		gainSlider.setSliderStyle(Slider::Rotary);

		// add some labels for the sliders..
		gainLabel.attachToComponent(&gainSlider, false);
		gainLabel.setFont(Font(11.0f));

		// add a label that will display the current timecode and status..
		addAndMakeVisible(timecodeDisplayLabel);
		timecodeDisplayLabel.setFont(Font(Font::getDefaultMonospacedFontName(), 15.0f, Font::plain));

		// set resize limits for this plug-in
		setResizeLimits(400, 200, 1024, 700);

		// set our component's initial size to be the last one that was stored in the filter's settings
		//setSize(owner.lastUIWidth, owner.lastUIHeight);

		//updateTrackProperties();

		// start a timer which will keep our timecode display updated
		//startTimerHz(30);
	}

	~SynthAudioProcessorEditor()
	{
	}

	//==============================================================================
	void paint(Graphics& g) override {
		g.setColour(backgroundColour);
		g.fillAll();
	}

	void resized() override {
		juce::Rectangle<int> r(getLocalBounds().reduced(8));


		r.removeFromTop(20);
		juce::Rectangle<int> sliderArea(r.removeFromTop(60));
		gainSlider.setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 2)));
	}

	void hostMIDIControllerIsAvailable(bool) override {
		//not sure what we need to do here - Victoria
	}

	void timerCallback()
	{
		//not sure what we need to do here - Victoria
	}

private:
	Slider gainSlider;

	Label timecodeDisplayLabel, gainLabel;
	//ScopedPointer<RotarySlider> gainSlider; - Don't think we will need this anymore - Victoria 
	Colour backgroundColour;

};

//===================================================================================

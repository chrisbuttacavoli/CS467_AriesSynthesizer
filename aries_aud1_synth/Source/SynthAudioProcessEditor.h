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

class SynthAudioProcessorEditor::RotarySlider : public Slider,
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
};

//=================================================================================

class SynthAudioProcessorEditor : public AudioProcessorEditor,
	private Timer
{
public:	
	//SynthAudioProcessorEditor(SynthAudioProcessorEditor&);

	SynthAudioProcessorEditor(SynthProcessor& owner) : AudioProcessorEditor(owner),
		//midiKeyboard(owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),
		timecodeDisplayLabel(String()),
		gainLabel(String(), "Throughput level:")
	{
		// add some sliders..
		addAndMakeVisible(gainSlider = new RotarySlider (*owner.gainParam));
		gainSlider->setSliderStyle(Slider::Rotary);

		// add some labels for the sliders..
		gainLabel.attachToComponent(gainSlider, false);
		gainLabel.setFont(Font(11.0f));

		// add a label that will display the current timecode and status..
		addAndMakeVisible(timecodeDisplayLabel);
		timecodeDisplayLabel.setFont(Font(Font::getDefaultMonospacedFontName(), 15.0f, Font::plain));

		// set resize limits for this plug-in
		setResizeLimits(400, 200, 1024, 700);

		// set our component's initial size to be the last one that was stored in the filter's settings
		setSize(owner.lastUIWidth,
			owner.lastUIHeight);

		updateTrackProperties();

		// start a timer which will keep our timecode display updated
		startTimerHz(30);
	}

	~SynthAudioProcessorEditor()
	{
	}

	//~SynthAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override {}
	void resized() override {}
	void timerCallback() override {}
	void hostMIDIControllerIsAvailable(bool) override {}
	void updateTrackProperties() {}

private:
	class RotarySlider;

	Label timecodeDisplayLabel, gainLabel;
	ScopedPointer<RotarySlider> gainSlider;
	Colour backgroundColour;

	//==============================================================================
	SynthAudioProcessorEditor& getProcessor() const
	{
		return static_cast<SynthAudioProcessorEditor&> (processor);
	}

	void updateTimecodeDisplay(AudioPlayHead::CurrentPositionInfo);
};

//===================================================================================

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

/*
==============================================================================

This file is part of the JUCE library.
Copyright (c) 2015 - ROLI Ltd.

Permission is granted to use this software under the terms of either:
a) the GPL v2 (or any later version)
b) the Affero GPL v3

Details of these licenses can be found at: www.gnu.org/licenses

JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

------------------------------------------------------------------------------

To release a closed-source product which uses JUCE, commercial licenses are
available: visit www.juce.com for more information.

==============================================================================
*/


//Using the generic editor so that we can use it for the plugins as well maybe? - Victoria
class GenericEditor : public AudioProcessorEditor,
	public Slider::Listener,
	private Timer
{
public:
	enum
	{
		kParamControlHeight = 40,
		kParamLabelWidth = 80,
		kParamSliderWidth = 300
	};

	GenericEditor(AudioProcessor& parent)
		: AudioProcessorEditor(parent),
		noParameterLabel("noparam", "No parameters available")
	{
		const OwnedArray<AudioProcessorParameter>& params = parent.getParameters();

		//Creates a slider for each parameter of the processor
		for (int i = 0; i < params.size(); ++i)
		{
			if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]))
			{
				Slider* aSlider;

				paramSliders.add(aSlider = new Slider(param->name));
				aSlider->setRange(param->range.start, param->range.end);
				aSlider->setSliderStyle(Slider::Rotary);	//setting the slider to be rotary
				aSlider->setValue(*param);

				aSlider->addListener(this);
				addAndMakeVisible(aSlider);

				Label* aLabel;
				paramLabels.add(aLabel = new Label(param->name, param->name));
				addAndMakeVisible(aLabel);
			}
		}

		noParameterLabel.setJustificationType(Justification::horizontallyCentred | Justification::verticallyCentred);
		noParameterLabel.setFont(noParameterLabel.getFont().withStyle(Font::italic));

		setSize(kParamSliderWidth + kParamLabelWidth,
			jmax(1, kParamControlHeight * paramSliders.size()));

		if (paramSliders.size() == 0)
			addAndMakeVisible(noParameterLabel);
		else
			startTimer(100);
	}

	~GenericEditor()
	{
	}

	void resized() override
	{
		juce::Rectangle<int> r = getLocalBounds();
		noParameterLabel.setBounds(r);

		for (int i = 0; i < paramSliders.size(); ++i)
		{
			juce::Rectangle<int> paramBounds = r.removeFromTop(kParamControlHeight);
			juce::Rectangle<int> labelBounds = paramBounds.removeFromLeft(kParamLabelWidth);

			paramLabels[i]->setBounds(labelBounds);
			paramSliders[i]->setBounds(paramBounds);
		}
	}

	void paint(Graphics& g) override
	{
		g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
		g.fillAll();
	}

	//==============================================================================
	void sliderValueChanged(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			*param = (float)slider->getValue();
	}

	void sliderDragStarted(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->beginChangeGesture();
	}

	void sliderDragEnded(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->endChangeGesture();
	}

private:
	void timerCallback() override
	{
		const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();

		for (int i = 0; i < params.size(); ++i)
		{
			if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]))
			{
				if (i < paramSliders.size())
					paramSliders[i]->setValue(*param);
			}
		}
	}

	AudioParameterFloat* getParameterForSlider(Slider* slider)
	{
		const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
		return dynamic_cast<AudioParameterFloat*> (params[paramSliders.indexOf(slider)]);
	}

	Label noParameterLabel;
	OwnedArray<Slider> paramSliders;
	OwnedArray<Label> paramLabels;

};

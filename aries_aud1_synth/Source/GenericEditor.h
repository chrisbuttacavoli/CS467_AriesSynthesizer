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
	private ComboBox::Listener,
	private Timer
{
public:
	enum
	{
		kParamControlHeight = 25,
		kParamLabelWidth = 60,
		kParamSliderWidth = 60,
	};

	GenericEditor(AudioProcessor& parent)
		: AudioProcessorEditor(parent),
		noParameterLabel("noparam", "No parameters available"),
		theParent(parent)
	{
		
		// This gets all parameters AudioParameterFloat and AudioProcessParameterWithID.
		// So we have to make another editor to loop through the value state tree for testing
		const OwnedArray<AudioProcessorParameter>& params = parent.getParameters();
		
		// Creates a rotary slider for each parameter of the processor
		for (int i = 0; i < params.size(); ++i)
		{
			if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]))
			{
				Slider* aSlider;

				paramSliders.add(aSlider = new Slider(param->name));
				aSlider->setRange(param->range.start, param->range.end);

				if (param->name.contains("EQ") == 1) {
					aSlider->setSliderStyle(Slider::LinearHorizontal);
				}
				if (param->name.contains("Master") == 1) {
					aSlider->setSliderStyle(Slider::LinearVertical);
				}
				else {
					aSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
				}

				aSlider->setValue(*param);

				addAndMakeVisible(aSlider);
				aSlider->addListener(this);

				Label* aLabel;
				paramLabels.add(aLabel = new Label(param->name, param->name));
				addAndMakeVisible(aLabel);

				controls.add(aSlider);
			}
			else if (const AudioParameterChoice* choiceParam = dynamic_cast<AudioParameterChoice*> (params[i]))
			{
				ComboBox * cbOsc;
				paramCombo.add(cbOsc = new ComboBox(choiceParam->name));
				//const String & theChoices;
				int i = 1;
				for (const juce::String * theChoice = choiceParam->choices.begin(); theChoice < choiceParam->choices.end(); theChoice++) {
					cbOsc->addItem(*theChoice, i);
					i++;
				}

				//cbOsc->setJustificationType(Justification::centred);
				addAndMakeVisible(cbOsc);
				cbOsc->addListener(this);

				controls.add(cbOsc);
			}

		}

		//sliderTree = new AudioProcessorValueTreeState::SliderAttachment()

		noParameterLabel.setJustificationType(Justification::horizontallyCentred | Justification::verticallyCentred);
		noParameterLabel.setFont(noParameterLabel.getFont().withStyle(Font::italic));
		
		setSize(kParamSliderWidth + kParamLabelWidth,
			jmax(1000, kParamControlHeight * paramSliders.size()));
		
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

		//creating the combo boxes
		Label * aLabel2 = new Label("Oscillators", "Oscillators");

		addAndMakeVisible(aLabel2);
		//aLabel2->setBounds(2, 10, (r.getWidth() / 4.8), 50);
		paramCombo[0]->setBounds(((r.getWidth() / 82)), 12, (r.getWidth() / 4.8), 25);
		paramCombo[1]->setBounds((240 + (r.getWidth() / 82)), 12, (r.getWidth() / 4.8), 25);
		paramCombo[2]->setBounds((480 + (r.getWidth() / 82)), 12, (r.getWidth() / 4.8), 25);
		paramCombo[3]->setBounds((720 + (r.getWidth() / 82)), 12, (r.getWidth() / 4.8), 25);

		//pitch 1
		paramLabels[0]->setBounds(((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 25);
		paramSliders[0]->setBounds(((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 50);

		//level 1
		paramLabels[1]->setBounds(((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 25);
		paramSliders[1]->setBounds(((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 50);

		//pitch 2
		paramLabels[2]->setBounds(240 + ((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 25);
		paramSliders[2]->setBounds(240 + ((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 50);
		//level 2
		paramLabels[3]->setBounds(240 + ((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 25);
		paramSliders[3]->setBounds(240 + ((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 50);

		//pitch 3
		paramLabels[4]->setBounds(480 + ((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 25);
		paramSliders[4]->setBounds(480 + ((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 50);
		//level 3
		paramLabels[5]->setBounds(480 + ((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 25);
		paramSliders[5]->setBounds(480 + ((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 50);

		//pitch 4
		paramLabels[6]->setBounds(720 + ((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 25);
		paramSliders[6]->setBounds(720 + ((r.getWidth() / 82)), 40, (r.getWidth() / 4.8), 50);
		//level 4
		paramLabels[7]->setBounds(720 + ((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 25);
		paramSliders[7]->setBounds(720 + ((r.getWidth() / 82)), 90, (r.getWidth() / 4.8), 50);

		/*
			ADSR stuff
		*/

		//attack
		paramLabels[8]->setBounds(240 + ((r.getWidth() / 82)), 370, (r.getWidth() / 8), 25);
		paramSliders[8]->setBounds(240 + ((r.getWidth() / 82)), 370, (r.getWidth() / 8), 50);
		//decay
		paramLabels[9]->setBounds(240 + ((r.getWidth() / 82)), 410, (r.getWidth() / 8), 25);
		paramSliders[9]->setBounds(240 + ((r.getWidth() / 82)), 410, (r.getWidth() / 8), 50);
		//sustain
		paramLabels[10]->setBounds(380 + ((r.getWidth() / 82)), 370, (r.getWidth() / 8), 25);
		paramSliders[10]->setBounds(380 + ((r.getWidth() / 82)), 370, (r.getWidth() / 8), 50);
		//release
		paramLabels[11]->setBounds(380 + ((r.getWidth() / 82)), 410, (r.getWidth() / 8), 25);
		paramSliders[11]->setBounds(380 + ((r.getWidth() / 82)), 410, (r.getWidth() / 8), 50);

		/*
			Filter stuff
		*/
		//type
		paramCombo[4]->setBounds(((r.getWidth() / 82)), 190, (r.getWidth() / 4.8), 20);
		//cutoff
		paramLabels[12]->setBounds(((r.getWidth() / 82)), 220, (r.getWidth() / 4.8), 25);
		paramSliders[12]->setBounds(((r.getWidth() / 82)), 220, (r.getWidth() / 4.8), 50);
		//res
		paramLabels[13]->setBounds(((r.getWidth() / 82)), 270, (r.getWidth() / 4.8), 25);
		paramSliders[13]->setBounds(((r.getWidth() / 82)), 270, (r.getWidth() / 4.8), 50);

		//distortion
		paramLabels[16]->setBounds(550 + ((r.getWidth() / 82)), 380, (r.getWidth() / 4.8), 25);
		paramSliders[16]->setBounds(550 + ((r.getWidth() / 82)), 380, (r.getWidth() / 4.8), 50);

		//noParameterLabel.setBounds(r);

		/*
			LFO stuff
		*/

		// LFO Combo Box
		paramCombo[5]->setBounds(((r.getWidth() / 82)), 370, (r.getWidth() / 4.8), 20);

		// LFO Level
		paramLabels[14]->setBounds(( (r.getWidth() / 82)), 390, (r.getWidth() / 4.8), 25);
		paramSliders[14]->setBounds(( (r.getWidth() / 82)), 390, (r.getWidth() / 4.8), 50);

		// LFO Frequency 
		paramLabels[15]->setBounds(( (r.getWidth() / 82)), 430, (r.getWidth() / 4.8), 25);
		paramSliders[15]->setBounds(( (r.getWidth() / 82)), 430, (r.getWidth() / 4.8), 50);

		/*
			EQ stuff
		*/
		// Low EQ Freq
		paramLabels[17]->setBounds((85 + (r.getWidth() / 6)), 200, (r.getWidth() / 6), 25);
		paramSliders[17]->setBounds((85 + (r.getWidth() / 6)), 200, (r.getWidth() / 6), 50);
		// Low EQ Q
		paramLabels[18]->setBounds((85 + (r.getWidth() / 6)), 240, (r.getWidth() / 6), 25);
		paramSliders[18]->setBounds((85 + (r.getWidth() / 6)), 240, (r.getWidth() / 6), 50);
		// Low EQ Level
		paramLabels[19]->setBounds((85 + (r.getWidth() / 6)), 280, (r.getWidth() / 6), 25);
		paramSliders[19]->setBounds((85 + (r.getWidth() / 6)), 280, (r.getWidth() / 6), 50);

		// Mid EQ Freq
		paramLabels[20]->setBounds((300 + (r.getWidth() / 6)), 200, (r.getWidth() / 6), 25);
		paramSliders[20]->setBounds((300 + (r.getWidth() / 6)), 200, (r.getWidth() / 6), 50);
		// Mid EQ Q
		paramLabels[21]->setBounds((300 + (r.getWidth() / 6)), 240, (r.getWidth() / 6), 25);
		paramSliders[21]->setBounds((300 + (r.getWidth() / 6)), 240, (r.getWidth() / 6), 50);
		// Mid EQ Level
		paramLabels[22]->setBounds((300 + (r.getWidth() / 6)), 280, (r.getWidth() / 6), 25);
		paramSliders[22]->setBounds((300 + (r.getWidth() / 6)), 280, (r.getWidth() / 6), 50);

		// Hi EQ Freq
		paramLabels[23]->setBounds((515 + (r.getWidth() / 6)), 200, (r.getWidth() / 6), 25);
		paramSliders[23]->setBounds((515 + (r.getWidth() / 6)), 200, (r.getWidth() / 6), 50);
		// Hi EQ Q
		paramLabels[24]->setBounds((515 + (r.getWidth() / 6)), 240, (r.getWidth() / 6), 25);
		paramSliders[24]->setBounds((515 + (r.getWidth() / 6)), 240, (r.getWidth() / 6), 50);
		// Hi EQ Level
		paramLabels[25]->setBounds((515 + (r.getWidth() / 6)), 280, (r.getWidth() / 6), 25);
		paramSliders[25]->setBounds((515 + (r.getWidth() / 6)), 280, (r.getWidth() / 6), 50);

		// MASTER OF THE PRECIOUS
		paramLabels[26]->setBounds((645 + (r.getWidth() / 6)), 360, (r.getWidth() / 6), 25);
		paramSliders[26]->setBounds((645 + (r.getWidth() / 6)), 360, (r.getWidth() / 6), 50);
	}

	void paint(Graphics& g) override
	{
		//g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
		g.setColour(Colour::fromRGB(0, 0, 0));

		g.fillAll();
		Image osc1Bg = ImageCache::getFromMemory(BinaryData::osc_1_final_png, BinaryData::osc_1_final_pngSize);
		g.drawImageAt(osc1Bg, 0, 0); 
		Image osc2Bg = ImageCache::getFromMemory(BinaryData::osc_2_final_png, BinaryData::osc_2_final_pngSize);
		g.drawImageAt(osc2Bg, 242, 0);
		Image osc3Bg = ImageCache::getFromMemory(BinaryData::osc_3_final_png, BinaryData::osc_3_final_pngSize);
		g.drawImageAt(osc3Bg, 484, 0);
		Image osc4Bg = ImageCache::getFromMemory(BinaryData::osc_4_final_png, BinaryData::osc_4_final_pngSize);
		g.drawImageAt(osc4Bg, 726, 0);
		Image adsr = ImageCache::getFromMemory(BinaryData::adsr_final_png, BinaryData::adsr_final_pngSize);
		g.drawImageAt(adsr, 242, 364);
		Image filters = ImageCache::getFromMemory(BinaryData::filters_final_png, BinaryData::filters_final_pngSize);
		g.drawImageAt(filters, 0, 182);
		Image distortion = ImageCache::getFromMemory(BinaryData::distortion_final_png, BinaryData::distortion_final_pngSize);
		g.drawImageAt(distortion, 524, 364);
		Image eq3 = ImageCache::getFromMemory(BinaryData::eq3_final_v_png, BinaryData::eq3_final_v_pngSize);
		g.drawImageAt(eq3, 242, 182); 
		Image lfo = ImageCache::getFromMemory(BinaryData::lfo_final_png, BinaryData::lfo_final_pngSize);
		g.drawImageAt(lfo, 0, 364);

		g.setColour(Colour(255, 0, 255));
		g.drawVerticalLine(440, 200, 345);
		g.drawVerticalLine(660, 200, 345);
	}

	//==============================================================================
	void sliderValueChanged(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider)) {

			*param = (float)slider->getValue();
			//for use with updating params in GUI thread on slider move
			//theParent.setParameterNotifyingHost(param->getParameterIndex() ,(float)slider->getValue());
		}
	}

	void sliderDragStarted(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->beginChangeGesture();
	}
	void setSliderSnapsToMousePosition(bool shouldSnapToMouse);
	void setScrollWheelEnabled(bool enabled);
	void setVelocityModeParameters(double sensitivity = 1.0,
		int threshold = 1,
		double offset = 0.0,
		bool userCanPressKeyToSwapMode = true);

	void sliderDragEnded(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->endChangeGesture();
	}

	void comboBoxChanged(juce::ComboBox *cb) override {

		if (AudioParameterChoice* param = getParameterForComboBox(cb))
			*param = cb->getSelectedItemIndex();
	}

private:
	// I believe this updates the GUI if we programically change the parameter values. This happens
	// on a timer, and we don't want to shove this code in the audio thread otherwise that would
	// consume too much CPU.
	void timerCallback() override
	{
		const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();

		for (int i = 0; i < controls.size(); ++i)
		{
			if (Slider* slider = dynamic_cast<Slider*> (controls[i]))
			{
				AudioParameterFloat* param = static_cast<AudioParameterFloat*> (params[i]);
				slider->setValue((double)*param, dontSendNotification);
			}
		}
	}

	AudioParameterFloat* getParameterForSlider(Slider* slider)
	{
		const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
		return dynamic_cast<AudioParameterFloat*> (params[controls.indexOf(slider)]);
	}

	AudioParameterChoice* getParameterForComboBox(ComboBox* cb)
	{
		const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
		return dynamic_cast<AudioParameterChoice*> (params[controls.indexOf(cb)]);
	}

	Label noParameterLabel;
	OwnedArray<Slider> paramSliders;
	OwnedArray<Label> paramLabels;
	OwnedArray<ComboBox> paramCombo;
	Array<Component*> controls;
	AudioProcessor& theParent;
	
	//ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> sliderTree;

// Adding this below to test passing params via ValueStateTree
//private:
};

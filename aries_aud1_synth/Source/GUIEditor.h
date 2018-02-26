/*
  ==============================================================================

    GUIEditor.h
    Created: 25 Feb 2018 5:39:24pm
    Author:  Chris.Buttacavoli

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthProcessor.h"

class GUIEditor : public AudioProcessorEditor,
public Slider::Listener,
private ComboBox::Listener,
private Timer
{
public:
	enum
	{
		kParamControlHeight = 25,
		kParamLabelWidth = 80,
		kParamSliderWidth = 100,
	};

	GUIEditor(AudioProcessor& parent) :
		AudioProcessorEditor(&parent),
		noParameterLabel("noparam", "No parameters available"),
		synthProcessor(parent)
	{
		//sliderAttach = new AudioProcessorValueTreeState::SliderAttachment(synthProcessor.stateTree, "Level1", level1Slider);
		
		level1Slider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
		level1Slider.setTextBoxStyle(Slider::TextBoxBelow, false, 75, 25);
		level1Slider.setRange(0.0f, 1.0f);
		level1Slider.setValue(0.5f);
		level1Slider.addListener(this);
		addAndMakeVisible(&level1Slider);

		// This gets all parameters AudioParameterFloat and AudioProcessParameterWithID.
		// So we have to make another editor to loop through the value state tree for testing
		//const OwnedArray<AudioProcessorParameter>& params = parent.getParameters();

		// Creates a rotary slider for each parameter of the processor
		//for (int i = 0; i < params.size(); ++i)
		//{
		//	if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]))
		//	{
		//		Slider* aSlider;

		//		paramSliders.add(aSlider = new Slider(param->name));
		//		aSlider->setRange(param->range.start, param->range.end);

		//		if (param->name.contains("EQ") == 1) {
		//			aSlider->setSliderStyle(Slider::LinearHorizontal);
		//		}
		//		else {
		//			aSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		//		}

		//		aSlider->setValue(*param);

		//		addAndMakeVisible(aSlider);
		//		aSlider->addListener(this);

		//		Label* aLabel;
		//		paramLabels.add(aLabel = new Label(param->name, param->name));
		//		addAndMakeVisible(aLabel);

		//		controls.add(aSlider);
		//	}
		//	else if (const AudioParameterChoice* choiceParam = dynamic_cast<AudioParameterChoice*> (params[i]))
		//	{
		//		ComboBox * cbOsc;
		//		paramCombo.add(cbOsc = new ComboBox(choiceParam->name));
		//		//const String & theChoices;
		//		int i = 1;
		//		for (const juce::String * theChoice = choiceParam->choices.begin(); theChoice < choiceParam->choices.end(); theChoice++) {
		//			cbOsc->addItem(*theChoice, i);
		//			i++;
		//		}

		//		//cbOsc->setJustificationType(Justification::centred);
		//		addAndMakeVisible(cbOsc);
		//		cbOsc->addListener(this);

		//		controls.add(cbOsc);
		//	}

		//}

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

	~GUIEditor()
	{
	}

	void resized() override
	{
		juce::Rectangle<int> r = getLocalBounds();
		level1Slider.setBounds(20, 115, (r.getWidth() / 4.8), 50);
		//creating the combo boxes
		//Label * aLabel2 = new Label("Oscillators", "Oscillators");

		//addAndMakeVisible(aLabel2);
		////aLabel2->setBounds(2, 10, (r.getWidth() / 4.8), 50);
		//paramCombo[0]->setBounds(20, 12, (r.getWidth() / 4.8), 50);
		//paramCombo[1]->setBounds(285, 12, (r.getWidth() / 4.8), 50);
		//paramCombo[2]->setBounds(550, 12, (r.getWidth() / 4.8), 50);
		//paramCombo[3]->setBounds(815, 12, (r.getWidth() / 4.8), 50);

		////pitch 1
		//paramLabels[0]->setBounds(20, 65, (r.getWidth() / 4.8), 25);
		//paramSliders[0]->setBounds(20, 65, (r.getWidth() / 4.8), 50);

		////level 1
		//paramLabels[1]->setBounds(20, 115, (r.getWidth() / 4.8), 25);
		//paramSliders[1]->setBounds(20, 115, (r.getWidth() / 4.8), 50);

		////pitch 2
		//paramLabels[2]->setBounds(285, 65, (r.getWidth() / 4.8), 25);
		//paramSliders[2]->setBounds(285, 65, (r.getWidth() / 4.8), 50);
		////level 2
		//paramLabels[3]->setBounds(285, 115, (r.getWidth() / 4.8), 25);
		//paramSliders[3]->setBounds(285, 115, (r.getWidth() / 4.8), 50);

		////pitch 3
		//paramLabels[4]->setBounds(550, 65, (r.getWidth() / 4.8), 25);
		//paramSliders[4]->setBounds(550, 65, (r.getWidth() / 4.8), 50);
		////level 3
		//paramLabels[5]->setBounds(550, 115, (r.getWidth() / 4.8), 25);
		//paramSliders[5]->setBounds(550, 115, (r.getWidth() / 4.8), 50);

		////pitch 4
		//paramLabels[6]->setBounds(815, 65, (r.getWidth() / 4.8), 25);
		//paramSliders[6]->setBounds(815, 65, (r.getWidth() / 4.8), 50);
		////level 4
		//paramLabels[7]->setBounds(815, 115, (r.getWidth() / 4.8), 25);
		//paramSliders[7]->setBounds(815, 115, (r.getWidth() / 4.8), 50);

		///*
		//ADSR stuff
		//*/

		////attack
		//paramLabels[8]->setBounds(675, 200, (r.getWidth() / 4.8), 25);
		//paramSliders[8]->setBounds(675, 200, (r.getWidth() / 4.8), 50);
		////decay
		//paramLabels[9]->setBounds(675, 250, (r.getWidth() / 4.8), 25);
		//paramSliders[9]->setBounds(675, 250, (r.getWidth() / 4.8), 50);
		////sustain
		//paramLabels[10]->setBounds(825, 225, (r.getWidth() / 4.8), 25);
		//paramSliders[10]->setBounds(825, 225, (r.getWidth() / 4.8), 50);
		////release
		//paramLabels[11]->setBounds(825, 275, (r.getWidth() / 4.8), 25);
		//paramSliders[11]->setBounds(825, 275, (r.getWidth() / 4.8), 50);

		///*
		//Filter stuff
		//*/
		////type
		//paramCombo[4]->setBounds(420, 210, (r.getWidth() / 4.8), 20);
		////cutoff
		//paramLabels[12]->setBounds(425, 225, (r.getWidth() / 4.8), 25);
		//paramSliders[12]->setBounds(425, 225, (r.getWidth() / 4.8), 50);
		////res
		//paramLabels[13]->setBounds(425, 265, (r.getWidth() / 4.8), 25);
		//paramSliders[13]->setBounds(425, 265, (r.getWidth() / 4.8), 50);

		////distortion
		//paramLabels[16]->setBounds(825, 380, (r.getWidth() / 4.8), 25);
		//paramSliders[16]->setBounds(825, 380, (r.getWidth() / 4.8), 50);

		////noParameterLabel.setBounds(r);

		///*
		//LFO stuff
		//*/

		//// LFO Combo Box
		//paramCombo[5]->setBounds(417, 355, (r.getWidth() / 4.8), 25);

		//// LFO Level
		//paramLabels[14]->setBounds(425, 380, (r.getWidth() / 4.8), 25);
		//paramSliders[14]->setBounds(425, 380, (r.getWidth() / 4.8), 50);

		//// LFO Frequency 
		//paramLabels[15]->setBounds(425, 415, (r.getWidth() / 4.8), 25);
		//paramSliders[15]->setBounds(425, 415, (r.getWidth() / 4.8), 50);

		///*
		//EQ stuff
		//*/
		//// Low EQ Freq
		//paramLabels[17]->setBounds(20, 200, (r.getWidth() / 4.8), 25);
		//paramSliders[17]->setBounds(20, 200, (r.getWidth() / 4.8), 50);
		//// Low EQ Q
		//paramLabels[18]->setBounds(20, 235, (r.getWidth() / 4.8), 25);
		//paramSliders[18]->setBounds(20, 235, (r.getWidth() / 4.8), 50);
		//// Low EQ Level
		//paramLabels[19]->setBounds(20, 270, (r.getWidth() / 4.8), 25);
		//paramSliders[19]->setBounds(20, 270, (r.getWidth() / 4.8), 50);

		//// Mid EQ Freq
		//paramLabels[20]->setBounds(20, 305, (r.getWidth() / 4.8), 25);
		//paramSliders[20]->setBounds(20, 305, (r.getWidth() / 4.8), 50);
		//// Mid EQ Q
		//paramLabels[21]->setBounds(20, 340, (r.getWidth() / 4.8), 25);
		//paramSliders[21]->setBounds(20, 340, (r.getWidth() / 4.8), 50);
		//// Mid EQ Level
		//paramLabels[22]->setBounds(20, 375, (r.getWidth() / 4.8), 25);
		//paramSliders[22]->setBounds(20, 375, (r.getWidth() / 4.8), 50);

		//// Hi EQ Freq
		//paramLabels[23]->setBounds(20, 415, (r.getWidth() / 4.8), 25);
		//paramSliders[23]->setBounds(20, 415, (r.getWidth() / 4.8), 50);
		//// Hi EQ Q
		//paramLabels[24]->setBounds(20, 450, (r.getWidth() / 4.8), 25);
		//paramSliders[24]->setBounds(20, 450, (r.getWidth() / 4.8), 50);
		//// Hi EQ Level
		//paramLabels[25]->setBounds(20, 480, (r.getWidth() / 4.8), 25);
		//paramSliders[25]->setBounds(20, 480, (r.getWidth() / 4.8), 50);
	}

	void paint(Graphics& g) override
	{
		//g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
		g.setColour(Colour::fromRGB(0, 0, 0));

		g.fillAll();
		Image osc1Bg = ImageCache::getFromMemory(BinaryData::Osc_Box_1_sl2_png, BinaryData::Osc_Box_1_sl2_pngSize);
		g.drawImageAt(osc1Bg, 0, 0);
		Image osc2Bg = ImageCache::getFromMemory(BinaryData::Osc_Box_2_sl2_png, BinaryData::Osc_Box_2_sl2_pngSize);
		g.drawImageAt(osc2Bg, 265, 0);
		Image osc3Bg = ImageCache::getFromMemory(BinaryData::Osc_Box_3_sl2_png, BinaryData::Osc_Box_3_sl2_pngSize);
		g.drawImageAt(osc3Bg, 530, 0);
		Image osc4Bg = ImageCache::getFromMemory(BinaryData::Osc_Box_4_sl2_png, BinaryData::Osc_Box_4_sl2_pngSize);
		g.drawImageAt(osc4Bg, 795, 0);
		Image adsr = ImageCache::getFromMemory(BinaryData::adsr_m_png, BinaryData::adsr_m_pngSize);
		g.drawImageAt(adsr, 652, 200);
		Image filters = ImageCache::getFromMemory(BinaryData::filters_png, BinaryData::filters_pngSize);
		g.drawImageAt(filters, 399, 200);
		Image distortion = ImageCache::getFromMemory(BinaryData::distortion_s_png, BinaryData::distortion_s_pngSize);
		g.drawImageAt(distortion, 805, 352);
		Image eq3 = ImageCache::getFromMemory(BinaryData::eq3_s2_png, BinaryData::eq3_s2_pngSize);
		g.drawImageAt(eq3, 0, 200);
		Image lfo = ImageCache::getFromMemory(BinaryData::lfo_s_png, BinaryData::lfo_s_pngSize);
		g.drawImageAt(lfo, 401, 352);
	}

	//==============================================================================
	void sliderValueChanged(Slider* slider) override
	{
		if (slider == &level1Slider) {
			//synthProcessor.
		}
		//if (AudioParameterFloat* param = getParameterForSlider(slider)) {

		//	*param = (float)slider->getValue();
		//	for use with updating params in GUI thread on slider move
		//	//theParent.setParameterNotifyingHost(param->getParameterIndex() ,(float)slider->getValue());
		//}
	}

	void sliderDragStarted(Slider* slider) override
	{
		/*if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->beginChangeGesture();*/
	}

	void setSliderSnapsToMousePosition(bool shouldSnapToMouse);
	void setScrollWheelEnabled(bool enabled);
	void setVelocityModeParameters(double sensitivity = 1.0,
		int threshold = 1,
		double offset = 0.0,
		bool userCanPressKeyToSwapMode = true);

	void sliderDragEnded(Slider* slider) override
	{
		/*if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->endChangeGesture();*/
	}

	void comboBoxChanged(juce::ComboBox *cb) override {
		/*if (AudioParameterChoice* param = getParameterForComboBox(cb))
			*param = cb->getSelectedItemIndex();*/
	}

private:
	// I believe this updates the GUI if we programically change the parameter values. This happens
	// on a timer, and we don't want to shove this code in the audio thread otherwise that would
	// consume too much CPU.
	void timerCallback() override
	{
		/*const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();

		for (int i = 0; i < controls.size(); ++i)
		{
			if (Slider* slider = dynamic_cast<Slider*> (controls[i]))
			{
				AudioParameterFloat* param = static_cast<AudioParameterFloat*> (params[i]);
				slider->setValue((double)*param, dontSendNotification);
			}
		}*/
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
	AudioProcessor& synthProcessor;

	// ValueStateTree
	//ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> sliderTree;
	ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> sliderAttach;
	Slider level1Slider;

	// Adding this below to test passing params via ValueStateTree
	//private:
};

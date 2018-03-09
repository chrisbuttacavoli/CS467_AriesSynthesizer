# CS467_Aries_AUD1
# Project Plan

## Aries Team Members:
Alexander Clairmont 
Christopher Buttacavoli 
Victoria Dorn 

## Introductions:
The Aries team will be constructing a digital synthesizer on the Windows platform with the JUCE framework. JUCE is an audio framework for development of applications, digital signal processing, and virtual instruments written in C++. Our team will also be utilizing the Maximilion audio library for DSP effects. Every member of our team has some experience with creating music or sounds. Our team is also fairly experienced with C/C++. None of our members has had a good chance to really try out audio programming in C/C++ environments yet. So, this project will be a great gateway into that field of work.

## User Perspective:
Our synthesizer will allow the user to create and record sounds with the use of a variety of oscillators, digital signal processing (DSP), and a keyboard input (either windows keyboard or MIDI keyboard). The synthesizer will allow the user to combine up to 4 different oscillators at once to create the basic sound wave that they would like to apply effects to. The synthesizer will be filled with several different DSP effects to apply to their sounds including distortion, equalization, compression, and filters (high pass, low pass, and band pass). The target user is an audio professional or an audio hobbyist. The user should have experience with synthesizers and digital audio effects. Therefore, the synthesizer should be made to look similar to existing tools.

## Client:
The class instructor, Benjamin Brewster, will be considered the client for this project. The clients requirements will be the requirements outlined by Victoria Dorn along with the requirements outlined within the class syllabus.

## Client Requirements:
* The creation of a Windows based digital synthesizer.
* It must be created within the JUCE framework.
    * There should be multi-oscillator support.
        * The user should be able to combine up to 4 different oscillators to form their basic audio waveform.
        * Oscillators options: sine wave, square wave, sawtooth wave, and noise
        * Allow volume adjustment of all oscillators
    * There should be at least 5 effects that can be applied to the basic audio waveform.
        * Effects: envelope generator, equalization (3 band), distortion, low frequency oscillator, filters (Low pass, high pass, and band pass)
            * Each effect should have at least 2 controllable parameters.
                * EX: Distortion should have a drive parameter and a level parameter
                * EX: Low Frequency Oscillator should have a parameter for the Low Frequency Oscillation (LFO) frequency and the LFO strength/level
* The synthesizer should be polyphonic (being able to play at least 5 sounds at once).
* The synthesizer should support MIDI keyboard input, click input from an onscreen keyboard, or use their PC keyboard as input.

## Sources:
We will be using the JUCE framework in addition to supplemental functions created by our team to create and power the synthesizer. As for the visual elements of our project, the JUCE framework contains a large amount of UI elements that we will be using for this project. Since this project is for educational purposes, we will be using the JUCE framework under their educational license.
We will also be using the Maximilion library for some DSP effects as well. Digital signal processing is comprised of a lot of really complicated math problems. Since we would like to create a quality, usable synthesizer more than anything, we have decided to spend more time making it as usable as possible instead of custom writing all of our audio effects. Finally, we are making use of the biquad filter source code found here: http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/

##Synth:
The synthesizer can be used by opening the AriesSynth.exe file found in the main directory of the project.
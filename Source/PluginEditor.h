/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class OscWriter2AudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener,
                                        public ToggleButton::Listener,
                                        public TextEditor::Listener,
                                        public Timer
{
public:
    enum
    {
        kParamSliderHeight = 40,
        kParamLabelWidth = 200,
        kParamSliderWidth = 320
    };
    
    OscWriter2AudioProcessorEditor (OscWriter2AudioProcessor&);
    ~OscWriter2AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override;
    void textEditorTextChanged (TextEditor& editor) override;
    void textEditorFocusLost (TextEditor& editor) override;
    
    void buttonClicked (Button* button) override{};
    void buttonStateChanged (Button* button) override;

private:
    void timerCallback() override;
    
    AudioProcessorParameter* getParameterForSlider (Slider* slider)
    {
        const juce::Array<AudioProcessorParameter*>& params = getAudioProcessor()->getParameters();
        return params[paramSliders.indexOf(slider)];
    }
    
    OwnedArray<Slider> paramSliders;
    OwnedArray<ToggleButton>    paramToggles;
    OwnedArray<TextEditor> paramLabels;
    OSCSender sender;
    TextEditor* oscHostField;
    TextEditor* oscPortField;
    TextEditor* generatorPreset;
    Label* presetlabel;
    std::vector<bool> flagChecker;
    float oldBpm;
    
    Image logo;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OscWriter2AudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscWriter2AudioProcessorEditor)
};

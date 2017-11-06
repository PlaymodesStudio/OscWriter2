/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OscWriter2AudioProcessorEditor::OscWriter2AudioProcessorEditor (OscWriter2AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    const OwnedArray<AudioProcessorParameter>& params = p.getParameters();
    for (int i = 0; i < params.size(); ++i)
    {
        if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*>(params[i]))
        {
            const bool isLevelMeter = (((param->category & 0xffff0000) >> 16) == 2);
            if (isLevelMeter)
                continue;
            
            Slider* aSlider;
            
            paramSliders.add (aSlider = new Slider (param->name));
            aSlider->setRange (param->range.start, param->range.end);
            aSlider->setSliderStyle (Slider::LinearHorizontal);
            aSlider->setValue (dynamic_cast<const AudioProcessorParameter*>(param)->getValue());
            
            aSlider->addListener (this);
            addAndMakeVisible (aSlider);
            
            TextEditor* aLabel;
            paramLabels.add (aLabel = new TextEditor (param->name));
            aLabel->setText(p.getOscAddressByIndex(i));
            aLabel->addListener (this);
            addAndMakeVisible (aLabel);
        }
    }

    oscHostField = new TextEditor("oscHost");
    oscPortField = new TextEditor("oscPort");
    oscHostField->setText(p.getOscHost());
    oscPortField->setText(p.getOscPort());
    oscHostField->addListener(this);
    oscPortField->addListener(this);
    addAndMakeVisible(oscHostField);
    addAndMakeVisible(oscPortField);
    
    setSize (kParamSliderWidth + kParamLabelWidth,
             jmax (1, kParamSliderHeight * paramSliders.size()) + kParamSliderHeight);
    

    startTimer(16);
    
    sender.connect(oscHostField->getText(), oscPortField->getText().getIntValue());
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
}

OscWriter2AudioProcessorEditor::~OscWriter2AudioProcessorEditor()
{
}

//==============================================================================
void OscWriter2AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
//
//    g.setColour (Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
    
}

void OscWriter2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> r = getLocalBounds();
    
    Rectangle<int> hostBounds = r.removeFromTop (kParamSliderHeight);
    Rectangle<int> portBounds = hostBounds.removeFromRight (kParamLabelWidth);
    hostBounds.setWidth(portBounds.getWidth());
    hostBounds.reduce(2, 10);
    portBounds.reduce(2, 10);
    oscHostField->setBounds(hostBounds);
    oscPortField->setBounds(portBounds);
    
    for (int i = 0; i < paramSliders.size(); ++i)
    {
        Rectangle<int> paramBounds = r.removeFromTop (kParamSliderHeight);
        Rectangle<int> labelBounds = paramBounds.removeFromLeft (kParamLabelWidth);
        labelBounds.reduce(0, 10);
        
        paramLabels[i]->setBounds (labelBounds);
        paramSliders[i]->setBounds (paramBounds);
    }
}

void OscWriter2AudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
    if (AudioProcessorParameter* param = params[paramSliders.indexOf(slider)])
    {
        if (slider->isMouseButtonDown())
            param->setValueNotifyingHost ((float) slider->getValue());
        else{
            param->setValue ((float) slider->getValue());
            sender.send(paramLabels[paramSliders.indexOf(slider)]->getText(), (float)param->getValue());
        }
    }
}

void OscWriter2AudioProcessorEditor::textEditorTextChanged(TextEditor &editor)
{
    const int editorIndex = paramLabels.indexOf(&editor);
    if(editorIndex != -1){
        processor.setOscAddress(editor.getText(), editorIndex);
    }
}

void OscWriter2AudioProcessorEditor::textEditorFocusLost(TextEditor &editor)
{
    if(paramLabels.indexOf(&editor) == -1){
        sender.disconnect();
        sender.connect(oscHostField->getText(), oscPortField->getText().getIntValue());
        processor.setOscHost(oscHostField->getText());
        processor.setOscPort(oscPortField->getText());
    }
}

void OscWriter2AudioProcessorEditor::timerCallback()
{
    const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
    for (int i = 0; i < params.size(); ++i)
    {
        if (const AudioProcessorParameter* param = params[i])
        {
            if (i < paramSliders.size())
                paramSliders[i]->setValue (param->getValue());
        }
    }
    if(processor.getFlag())
    {
        for(int  i = 0; i < paramLabels.size(); i ++)
        {
            paramLabels[i]->setText(processor.getOscAddressByIndex(i));
        }
        if(oscHostField->getText() != processor.getOscHost() || oscPortField->getText() != processor.getOscPort()){
            sender.connect(processor.getOscHost(), processor.getOscPort().getIntValue());
        }
        oscHostField->setText(processor.getOscHost());
        oscPortField->setText(processor.getOscPort());
        processor.setFlagFalse();
    }
}

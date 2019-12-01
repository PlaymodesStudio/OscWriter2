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
        
        if (const AudioParameterBool* param = dynamic_cast<AudioParameterBool*>(params[i]))
        {
            ToggleButton* aToggle;
            
            paramToggles.add(aToggle = new ToggleButton(param->name));
            aToggle->setEnabled(false);
            aToggle->addListener(this);
            addAndMakeVisible(aToggle);
            
            TextEditor* aLabel;
            paramLabels.add (aLabel = new TextEditor (param->name));
            aLabel->setText(p.getOscAddressByIndex(i));
            aLabel->addListener (this);
            addAndMakeVisible (aLabel);
        }
    }
    
    flagChecker.resize(paramLabels.size()-paramSliders.size(), false);

    oscHostField = new TextEditor("oscHost");
    oscPortField = new TextEditor("oscPort");
    oscHostField->setText(p.getOscHost());
    oscPortField->setText(p.getOscPort());
    oscHostField->addListener(this);
    oscPortField->addListener(this);
    addAndMakeVisible(oscHostField);
    addAndMakeVisible(oscPortField);
    
    generatorPreset = new TextEditor("Preset");
    generatorPreset->setText(p.getPresetName());
    generatorPreset->addListener(this);
    addAndMakeVisible(generatorPreset);
    
    presetlabel = new Label("Preset");
    presetlabel->setText("Preset ->", juce::dontSendNotification);
    addAndMakeVisible(presetlabel);
    
    setSize (kParamSliderWidth + kParamLabelWidth,
             jmax (1, kParamSliderHeight * (paramLabels.size()+1)) + kParamSliderHeight);
    

    startTimer(16);
    
    sender.connect(oscHostField->getText(), oscPortField->getText().getIntValue());
    logo = ImageCache::getFromMemory(BinaryData::anagrama_sol_2_png, BinaryData::anagrama_sol_2_pngSize);
    
    oldBpm = 0.0;
    
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
    
//    g.drawImageAt(logo, getLocalBounds().getCentreX(), 10);
    g.drawImage(logo, getLocalBounds().getCentreX() - (20), 5, 40, 40, 0, 0, logo.getWidth(), logo.getHeight());
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
    Rectangle<int> presetBounds = r.removeFromTop(kParamSliderHeight);
    Rectangle<int> presetLabelBounds = presetBounds.removeFromLeft(70);
    hostBounds.setWidth(portBounds.getWidth());
    hostBounds.reduce(2, 10);
    portBounds.reduce(2, 10);
    presetBounds.reduce(2, 10);
    presetLabelBounds.reduce(2, 10);
    oscHostField->setBounds(hostBounds);
    oscPortField->setBounds(portBounds);
    generatorPreset->setBounds(presetBounds);
    presetlabel->setBounds(presetLabelBounds);
    
    for (int i = 0; i < paramSliders.size(); ++i)
    {
        Rectangle<int> paramBounds = r.removeFromTop (kParamSliderHeight);
        Rectangle<int> labelBounds = paramBounds.removeFromLeft (kParamLabelWidth);
        labelBounds.reduce(0, 10);
        
        paramLabels[i]->setBounds (labelBounds);
        paramSliders[i]->setBounds (paramBounds);
    }
    for (int i = paramSliders.size(); i < paramLabels.size(); i++){
        Rectangle<int> paramBounds = r.removeFromTop (kParamSliderHeight);
        Rectangle<int> labelBounds = paramBounds.removeFromLeft (kParamLabelWidth);
        labelBounds.reduce(0, 10);
        
        paramToggles[i-paramSliders.size()]->setBounds(paramBounds);
        paramLabels[i]->setBounds (labelBounds);
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
            String oscAddress = paramLabels[paramSliders.indexOf(slider)]->getText();
            if(oscAddress.isNotEmpty()){
                sender.send(oscAddress, (float)param->getValue());
            }
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
        if(generatorPreset == &editor){
            processor.setPresetName(generatorPreset->getText());
        }else{
            sender.disconnect();
            sender.connect(oscHostField->getText(), oscPortField->getText().getIntValue());
            processor.setOscHost(oscHostField->getText());
            processor.setOscPort(oscPortField->getText());
        }
    }
}

void OscWriter2AudioProcessorEditor::buttonStateChanged(Button* button)
{
    const int index = paramToggles.indexOf(dynamic_cast<const ToggleButton*>(button));
    if(AudioProcessorParameter* param = getAudioProcessor()->getParameters()[index + paramSliders.size()]){
        if (const AudioParameterBool* boolparam = dynamic_cast<AudioParameterBool*>(param))
        {
            param->setValue((float)button->getToggleState());
        }
    }
}

void OscWriter2AudioProcessorEditor::timerCallback()
{
    const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
    for (int i = 0; i < params.size(); ++i)
    {
        if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*>(params[i]))
        {
            if (i < paramSliders.size())
                paramSliders[i]->setValue (param->get());
        }
        
        if (const AudioParameterBool* param = dynamic_cast<AudioParameterBool*>(params[i]))
        {
            if(param->get() == true){
                if(flagChecker[i-paramSliders.size()] == false){
                    if(paramLabels[i]->getText() != ""){
                        sender.send(paramLabels[i]->getText());
                    }
                    flagChecker[i-paramSliders.size()] = true;
                }
            }
            else{
                flagChecker[i-paramSliders.size()] = false;
            }
        }
    }
    float newBpm = processor.getBpm();
    if(newBpm != oldBpm){
        sender.send("/bpm", newBpm);
        oldBpm = newBpm;
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
        generatorPreset->setText(processor.getPresetName());
        sender.send("/presetLoad/" + generatorPreset->getText(), 0);
        sender.send("/bpm", newBpm);
        //Send all parameters
        for(auto &slider : paramSliders){
            String oscAddress = paramLabels[paramSliders.indexOf(slider)]->getText();
            if(oscAddress.isNotEmpty()){
                sender.send(oscAddress, (float)slider->getValue());
            }
        }
        processor.setFlagFalse();
    }
}

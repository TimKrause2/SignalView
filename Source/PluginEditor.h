/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class TestRenderer;

class SpectrumAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     public juce::Slider::Listener,
                                     public juce::Button::Listener
{
public:
    SpectrumAudioProcessorEditor (SpectrumAudioProcessor&);
    ~SpectrumAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider *slider) override;
    void buttonClicked(juce::Button *button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpectrumAudioProcessor& audioProcessor;

    juce::ImageComponent image;
    juce::OpenGLContext  glcontext;
    std::unique_ptr<TestRenderer> glrenderer;
    
    friend class TestRenderer;

    juce::Slider rangeSlider;
    juce::Slider widthSlider;
    juce::ToggleButton logButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAudioProcessorEditor)
};

class TestRenderer : public juce::OpenGLRenderer
{
public:
    TestRenderer(SpectrumAudioProcessorEditor &editor);
    void newOpenGLContextCreated () override;
    void renderOpenGL () override;
    void openGLContextClosing () override;
    
private:
    SpectrumAudioProcessorEditor &editor;
};





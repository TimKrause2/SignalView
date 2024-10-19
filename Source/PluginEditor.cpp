/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpectrumAudioProcessorEditor::SpectrumAudioProcessorEditor (SpectrumAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    setResizable(true, true);
    
    addAndMakeVisible(image);
    glrenderer.reset(new TestRenderer(*this));
    glcontext.setRenderer(glrenderer.get());
    glcontext.attachTo(image);
    glcontext.setContinuousRepainting(true);
    
    rangeSlider.addListener(this);
    rangeSlider.setSliderStyle(juce::Slider::TwoValueVertical);
    rangeSlider.setRange(-180.0, 0.0);
    rangeSlider.setMinAndMaxValues(-180.0, 0.0);
    addAndMakeVisible(rangeSlider);
    
    addAndMakeVisible(widthSlider);
    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    widthSlider.setRange(0.1, 1.0);
    widthSlider.setValue(1.0);
    widthSlider.addListener(this);

}

SpectrumAudioProcessorEditor::~SpectrumAudioProcessorEditor()
{
    glcontext.detach();
}

//==============================================================================
void SpectrumAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void SpectrumAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto widthSliderHeight = 50;
    widthSlider.setBounds(area.removeFromBottom(widthSliderHeight));
    auto sliderWidth = 50;
    rangeSlider.setBounds(area.removeFromRight(sliderWidth));
    image.setBounds(area);
}

void SpectrumAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if(slider == &rangeSlider){
        auto dB_min = rangeSlider.getMinValue();
        auto dB_max = rangeSlider.getMaxValue();
        if(audioProcessor.spectrum)
            audioProcessor.spectrum->SetdBLimits(dB_min, dB_max);
    }else if(slider == &widthSlider){
        float width = widthSlider.getValue();
        if(audioProcessor.spectrum)
            audioProcessor.spectrum->SetWidth(width);
    }
}


TestRenderer::TestRenderer(SpectrumAudioProcessorEditor &editor):
    editor(editor)
{
}

void TestRenderer::newOpenGLContextCreated()
{
    editor.audioProcessor.spectrum->GLInit();
}

void TestRenderer::renderOpenGL()
{
    editor.audioProcessor.spectrum->Render();
}

void TestRenderer::openGLContextClosing()
{
    editor.audioProcessor.spectrum->GLDestroy();
}


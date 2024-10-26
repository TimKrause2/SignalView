/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpectrumAudioProcessorEditor::SpectrumAudioProcessorEditor (SpectrumAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    logButton("Log")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 800);
    setResizable(true, true);
    
    addAndMakeVisible(image);
    glrenderer.reset(new TestRenderer(*this));
    glcontext.setRenderer(glrenderer.get());
    glcontext.attachTo(image);
    glcontext.setContinuousRepainting(true);
    
    rangeSlider.addListener(this);
    rangeSlider.setSliderStyle(juce::Slider::TwoValueVertical);
    rangeSlider.setRange(-180.0, 0.0, 3.0);
    rangeSlider.setMinAndMaxValues(-180.0, 0.0);
    rangeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 15, 15);
    rangeSlider.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(rangeSlider);
    
    addAndMakeVisible(widthSlider);
    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    float nyquistRate = p.getSampleRate()/2.0;
    widthSlider.setRange(1000.0, nyquistRate);
    widthSlider.setValue(nyquistRate);
    widthSlider.setNumDecimalPlacesToDisplay(0);
    widthSlider.addListener(this);

    addAndMakeVisible(logButton);
    logButton.addListener(this);
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
    auto widthSliderHeight = 35;
    auto bottom = area.removeFromBottom(widthSliderHeight);
    logButton.setBounds(bottom.removeFromLeft(80));
    widthSlider.setBounds(bottom);
    auto sliderWidth = 40;
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
        float frequency = widthSlider.getValue();
        if(audioProcessor.spectrum)
            audioProcessor.spectrum->SetWidth(frequency);
    }
}

void SpectrumAudioProcessorEditor::buttonClicked(juce::Button *button)
{
    if(button == &logButton){
        bool log = logButton.getToggleState();
        audioProcessor.spectrum->SetFrequency(log);
        if(log){
            widthSlider.setVisible(false);
            audioProcessor.spectrum->SetWidth(audioProcessor.getSampleRate()/2.0);
        }else{
            widthSlider.setVisible(true);
            float frequency = widthSlider.getValue();
            audioProcessor.spectrum->SetWidth(frequency);
        }
    }
}



TestRenderer::TestRenderer(SpectrumAudioProcessorEditor &editor):
    editor(editor)
{
}

#ifdef DEBUG
#include <stdio.h>

void MessageCallback( GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

#endif


void TestRenderer::newOpenGLContextCreated()
{
#ifdef DEBUG
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
#endif
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


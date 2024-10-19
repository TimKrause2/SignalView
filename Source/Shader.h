/*
  ==============================================================================

    Shader.h
    Created: 18 Jan 2024 8:59:01pm
    Author:  tim

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce::gl;

GLuint LoadShader(GLenum type, const char *shaderSrc);
GLuint LoadProgram(const char *vertShaderSrc, const char *fragShaderSrc);



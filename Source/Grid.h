/*
  ==============================================================================

    Grid.h
    Created: 23 Oct 2024 3:13:01pm
    Author:  tim



    Log view lines
    4 lines - border
    4 lines - decade markers
    3*8 + 1 = 25 lines - linear markers
    33 lines - total


  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <glm/glm.hpp>

using namespace juce::gl;

#define N_LOG_BORDER 4
#define N_LOG_DECADE 4
#define N_LOG_LINEAR 25
#define N_LOG_TOTAL  33

class Grid
{
    GLuint program;
    GLint color_loc;
    GLint mvp_loc;
    GLuint log_vbo;
    GLuint linear_vbo;
    GLuint log_vao;
    GLuint linear_vao;
    int   Nfft;
    bool  log;
    float dB_top;
    float dB_bottom;
    float view_width;
    float sample_rate;
    glm::vec4 log_border_color;
    glm::vec4 log_decade_color;
    glm::vec4 log_linear_color;

    void ProgramLoad(void);
    void ProgramDestroy(void);

    void InitializeLinear(void);
    void InitializeLog(void);

    float x_LogDisplacement(float frequency);
    float x_LinearDisplacement(float frequency);

public:
    Grid(int Nfft, float sample_rate);
    ~Grid(void);
    void SetFrequency(bool log=false);
    void SetLimits(float dB_top, float dB_bottom);
    void SetViewWidth(float view_width);
    void Draw(void);
};

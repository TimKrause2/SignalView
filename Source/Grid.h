/*
  ==============================================================================

    Grid.h
    Created: 23 Oct 2024 3:13:01pm
    Author:  tim

    Magnitude dB lines 0dB to -180dB
    8 lines - 24dB
    16 lines - 12dB
    31 lines - 6dB
    61 lines - 3dB
    108 lines - total

    Log view frequency lines
    4 lines - border
    4 lines - decade markers
    3*8 + 1 = 25 lines - linear markers
    33 lines - total


  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <glm/glm.hpp>
#include "Font.h"

using namespace juce::gl;

#define N_LOG_BORDER 4
#define N_LOG_DECADE 4
#define N_LOG_LINEAR 25
#define N_LOG_TOTAL  33
#define N_DB_24 8
#define N_DB_12 16
#define N_DB_6 31
#define N_DB_3 61
#define N_DB_TOTAL 116
#define DB_OFFSET_24 0
#define DB_OFFSET_12 N_DB_24
#define DB_OFFSET_6 (DB_OFFSET_12 + N_DB_12)
#define DB_OFFSET_3 (DB_OFFSET_6 + N_DB_6)

class Grid
{
    GLuint program;
    GLint color_loc;
    GLint mvp_loc;
    GLuint log_vbo;
    GLuint dB_vbo;
    GLuint linear_vbo;
    GLuint log_vao;
    GLuint dB_vao;
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
    glm::vec4 dB_color;
    FreeTypeFont font;
    FreeTypeFont dB_font;
    int font_height;

    void ProgramLoad(void);
    void ProgramDestroy(void);

    void InitializeLinear(void);
    void InitializeLog(void);
    void InitializedB(void);

    float x_LogDisplacement(float frequency);
    float x_LinearDisplacement(float frequency);

    void DrawLogFrequencyText(float frequency, const char *text,
                              int view_width, int view_height);

    float y_dBDisplacement(float dB);
    void Draw_dB(void);

public:
    Grid(int Nfft, float sample_rate);
    ~Grid(void);
    void SetFrequency(bool log=false);
    void SetLimits(float dB_top, float dB_bottom);
    void SetViewWidth(float view_width);
    void Draw(void);
};

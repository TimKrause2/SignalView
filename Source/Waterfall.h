/*
  ==============================================================================

    Waterfall.h
    Created: 19 Jan 2024 10:11:06am
    Author:  tim

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <glm/glm.hpp>

using namespace juce::gl;

class Waterfall
{
private:
    bool quadsInitialized;
    int  Npoints;
    int  Nlines;
    bool texture_phase;
    int  line;
    float view_width;
    float view_height;
    float dB_min;
    float dB_max;
    std::unique_ptr<unsigned char[]> pixels;
    GLuint textures[2];
    GLuint current_tex;
    GLuint trailing_tex;
    GLuint colormap_tex;
    GLuint vbos[2];
    GLuint vaos[2];
    GLuint program;
    GLint mvp_loc;
    GLint s_texture_loc;
    GLint s_colormap_loc;
    
    void InitQuads(void);
    void DeleteQuads(void);
public:
    Waterfall(int Npoints, int Nlines);
    ~Waterfall();
    
    void SetViewWidth(float width);
    void SetViewHeight(float height);
    void SetdBLimits(float dB_min, float dB_max);
    void InsertLine(float *data);
    void Render(void);
};

struct Attributes
{
    glm::vec2 vertex;
    glm::vec2 texel;
};

struct Pixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};




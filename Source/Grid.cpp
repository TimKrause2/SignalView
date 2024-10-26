/*
  ==============================================================================

    Grid.cpp
    Created: 23 Oct 2024 3:13:01pm
    Author:  tim

  ==============================================================================
*/

#include "Grid.h"
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define VERTEX_LOC 0

void Grid::ProgramLoad(void)
{
    const char *vertShaderSrc =
        "#version 460\n"
        "layout(location=0) in vec2 a_xy;\n"
        "uniform mat4 mvp;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = mvp*vec4(a_xy,0.0,1.0);\n"
        "}\n";

    const char *fragShaderSrc =
        "#version 460\n"
        "layout(location = 0) out vec4 f_color;\n"
        "uniform vec4 color;\n"
        "void main()\n"
        "{\n"
        "   f_color = color;\n"
        "}\n";

    program = LoadProgram(vertShaderSrc, fragShaderSrc);
    if(!program){
        printf("Grid.cpp: Error, couldn't load program.\n");
        return;
    }

    color_loc = glGetUniformLocation(program, "color");
    mvp_loc = glGetUniformLocation(program, "mvp");
}

void Grid::ProgramDestroy(void)
{
    glDeleteProgram(program);
}

void Grid::InitializeLinear(void)
{

}

void Grid::InitializeLog(void)
{
    glGenBuffers(1, &log_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, log_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*N_LOG_TOTAL*2,
                 NULL, GL_STATIC_DRAW);
    glm::vec2 *vertices = (glm::vec2*)glMapBufferRange(
        GL_ARRAY_BUFFER,
        0, sizeof(glm::vec2)*N_LOG_TOTAL*2,
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    int iv=0;
    // initialize the border lines
    // top line
    vertices[iv].x = 0.0f;
    vertices[iv].y = 1.0f;
    vertices[iv+1].x = 1.0f;
    vertices[iv+1].y = 1.0f;
    // left line
    vertices[iv+2].x = 0.0f;
    vertices[iv+2].y = 0.0f;
    vertices[iv+3].x = 0.0f;
    vertices[iv+3].y = 1.0f;
    // right line
    vertices[iv+4].x = 1.0f;
    vertices[iv+4].y = 0.0f;
    vertices[iv+5].x = 1.0f;
    vertices[iv+5].y = 1.0f;
    // bottom line
    vertices[iv+6].x = 0.0f;
    vertices[iv+6].y = 0.0f;
    vertices[iv+7].x = 1.0f;
    vertices[iv+7].y = 0.0f;

    iv+=8;
    // initialize the decade lines
    for(int d=1;d<=4;d++){
        float f = powf(10.0f, (float)d);
        float x = x_LogDisplacement(f);
        vertices[iv].x = x;
        vertices[iv].y = 0.0f;
        vertices[iv+1].x = x;
        vertices[iv+1].y = 1.0f;
        iv+=2;
    }

    // initialize the linear lines
    for(int d=1;d<=4;d++){
        float dec_base = powf(10.0f, (float)d);
        int l_range = (d==4)?2:9;
        for(int l=2;l<=l_range;l++){
            float f = dec_base * l;
            float x = x_LogDisplacement(f);
            vertices[iv].x = x;
            vertices[iv].y = 0.0f;
            vertices[iv+1].x = x;
            vertices[iv+1].y = 1.0f;
            iv+=2;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glGenVertexArrays(1, &log_vao);
    glBindVertexArray(log_vao);
    glBindBuffer(GL_ARRAY_BUFFER, log_vbo);
    glVertexAttribPointer(VERTEX_LOC, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(VERTEX_LOC);
    glBindVertexArray(0);

}

void Grid::InitializedB(void)
{
    glGenBuffers(1, &dB_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, dB_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*N_DB_TOTAL*2,
                 NULL, GL_STATIC_DRAW);
    glm::vec2 *vertices = (glm::vec2*)glMapBufferRange(
        GL_ARRAY_BUFFER,
        0, sizeof(glm::vec2)*N_DB_TOTAL*2,
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    int iv=0;
    // initialize the 24dB lines
    float dB = 0.0f;
    for(int l=0;l<N_DB_24;l++){
        vertices[iv].x = 0.0f;
        vertices[iv].y = dB;
        vertices[iv+1].x = 1.0f;
        vertices[iv+1].y = dB;
        dB -= 24.0f;
        iv += 2;
    }
    // initialize the 12dB lines
    dB = 0.0f;
    for(int l=0;l<N_DB_12;l++){
        vertices[iv].x = 0.0f;
        vertices[iv].y = dB;
        vertices[iv+1].x = 1.0f;
        vertices[iv+1].y = dB;
        dB -= 12.0f;
        iv += 2;
    }
    // initialize the 6dB lines
    dB = 0.0f;
    for(int l=0;l<N_DB_6;l++){
        vertices[iv].x = 0.0f;
        vertices[iv].y = dB;
        vertices[iv+1].x = 1.0f;
        vertices[iv+1].y = dB;
        dB -= 6.0f;
        iv += 2;
    }
    // initialize the 3dB lines
    dB = 0.0f;
    for(int l=0;l<N_DB_3;l++){
        vertices[iv].x = 0.0f;
        vertices[iv].y = dB;
        vertices[iv+1].x = 1.0f;
        vertices[iv+1].y = dB;
        dB -= 3.0f;
        iv += 2;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glGenVertexArrays(1, &dB_vao);
    glBindVertexArray(dB_vao);
    glBindBuffer(GL_ARRAY_BUFFER, dB_vbo);
    glVertexAttribPointer(VERTEX_LOC, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(VERTEX_LOC);
    glBindVertexArray(0);
}

float Grid::x_LogDisplacement(float frequency)
{
    float f1 = sample_rate/Nfft;
    float alpha2 = logf(2.0f)/logf(Nfft/2.0f);
    float beta = alpha2/(1.0f+alpha2);
    float x;
    if(frequency<f1){
        x = beta*frequency/f1;
    }else{
        float fn = sample_rate/2.0f;
        float alpha_f = logf(frequency/f1)/logf(fn/f1);
        x = beta + alpha_f*(1.0f - beta);
    }
    return x;
}

float Grid::x_LinearDisplacement(float frequency)
{
    return frequency/(sample_rate/2.0f);
}

Grid::Grid(int Nfft, float sample_rate):
    sample_rate(sample_rate),
    Nfft(Nfft),
    log(false),
    dB_top(0.0f),
    dB_bottom(-180.0f),
    view_width(1.0f),
    font_height(12)
{
    ProgramLoad();

    InitializeLog();
    InitializedB();

    glm::vec3 log_hsv_border(0.0f, 0.0f, 0.75f);
    glm::vec3 log_hsv_decade(0.0f, 0.0f, 0.5f);
    glm::vec3 log_hsv_linear(0.0f, 0.0f, 0.25f);
    glm::vec3 log_rgb_border = glm::rgbColor(log_hsv_border);
    glm::vec3 log_rgb_decade = glm::rgbColor(log_hsv_decade);
    glm::vec3 log_rgb_linear = glm::rgbColor(log_hsv_linear);
    log_border_color = glm::vec4(log_rgb_border, 1.0f);
    log_decade_color = glm::vec4(log_rgb_decade, 1.0f);
    log_linear_color = glm::vec4(log_rgb_linear, 1.0f);
    
    FT_Open_Args args;
    args.flags = FT_OPEN_MEMORY;
    args.memory_base = (const FT_Byte*)BinaryData::sui_generis_rg_otf;
    args.memory_size = BinaryData::sui_generis_rg_otfSize;

    glm::vec3 hsv_font_color(0.0f, 0.0f, 0.6f);
    glm::vec3 hsv_outl_color(0.0f, 0.0f, 0.25f);
    glm::vec3 rgb_font_color = glm::rgbColor(hsv_font_color);
    glm::vec3 rgb_outl_color = glm::rgbColor(hsv_outl_color);
    glm::vec4 font_color = glm::vec4(rgb_font_color, 1.0f);
    glm::vec4 outl_color = glm::vec4(rgb_outl_color, 1.0f);
    font.LoadOutline(&args, font_height, font_color, outl_color, 0.25f);

    glm::vec3 hsv_dB_color(0.0f, 0.75f, 0.6f);
    glm::vec3 hsv_dB_outl_color(0.0f, 0.75f, 0.25f);
    glm::vec3 rgb_dB_color = glm::rgbColor(hsv_dB_color);
    glm::vec3 rgb_dB_outl_color = glm::rgbColor(hsv_dB_outl_color);
    dB_color = glm::vec4(rgb_dB_color, 1.0f);
    glm::vec4 dB_outl_color(rgb_dB_outl_color, 1.0f);
    dB_font.LoadOutline(&args, font_height, dB_color, dB_outl_color, 0.25f);
}

Grid::~Grid(void)
{
    ProgramDestroy();
    glDeleteBuffers(1, &log_vbo);
    glDeleteVertexArrays(1, &log_vao);
    glDeleteBuffers(1, &dB_vbo);
    glDeleteVertexArrays(1, &dB_vao);
}

void Grid::SetFrequency(bool log)
{
    Grid::log = log;
}

void Grid::SetLimits(float dB_top, float dB_bottom)
{
    Grid::dB_top = dB_top;
    Grid::dB_bottom = dB_bottom;
}

void Grid::SetViewWidth(float view_width)
{
    Grid::view_width = view_width;
}

void Grid::DrawLogFrequencyText(float frequency, const char *text,
                               int view_width, int view_height)
{
    float x = x_LogDisplacement(frequency);
    double xs = x*view_width + 2.0;
    double ys = view_height - font_height - 2.0;
    font.Printf(xs, ys, text);
}

float Grid::y_dBDisplacement(float dB)
{
    return (dB-dB_bottom)/(dB_top-dB_bottom);
}

void Grid::Draw_dB(void)
{
    float delta_dB = dB_top - dB_bottom;
    if(delta_dB==0.0f)
        return;
    int group_offset;
    int i0;
    int i1;
    int Nlines;
    float dB_per_line;
    float range_per_line = delta_dB/7.5f;
    if(range_per_line < 3.0f){
        group_offset = DB_OFFSET_3;
        dB_per_line = 3.0f;
    }else if(range_per_line < 6.0f){
        group_offset = DB_OFFSET_6;
        dB_per_line = 6.0f;
    }else if(range_per_line < 12.0f){
        group_offset = DB_OFFSET_12;
        dB_per_line = 12.0f;
    }else{
        group_offset = DB_OFFSET_24;
        dB_per_line = 24.0f;
    }
    i0 = (int)ceilf(-dB_top/dB_per_line);
    i1 = (int)floorf(-dB_bottom/dB_per_line);
    Nlines = i1 - i0 + 1;

    float top = dB_top;
    float bottom = dB_bottom;
    float left = 0.0f;
    float right = 1.0f;
    float near = 1.0f;
    float far = -1.0f;
    glm::mat4 mvp = glm::ortho(left, right, bottom, top, near, far);
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(dB_vao);
    glUniform4fv(color_loc, 1, glm::value_ptr(dB_color));
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, (group_offset+i0)*2, Nlines*2);
}

void Grid::Draw(void)
{
    glUseProgram(program);

    Draw_dB();

    if(log){
        float top = 1.0f;
        float bottom = 0.0f;
        float left = 0.0f;
        float right = view_width;
        float near = 1.0f;
        float far = -1.0f;
        glm::mat4 mvp = glm::ortho(left, right, bottom, top, near, far);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(log_vao);
        glUniform4fv(color_loc, 1, glm::value_ptr(log_border_color));
        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, 0, N_LOG_BORDER*2);
        glUniform4fv(color_loc, 1, glm::value_ptr(log_decade_color));
        glLineWidth(1.0f);
        glDrawArrays(GL_LINES, N_LOG_BORDER*2, N_LOG_DECADE*2);
        glUniform4fv(color_loc, 1, glm::value_ptr(log_linear_color));
        glDrawArrays(GL_LINES, (N_LOG_BORDER+N_LOG_DECADE)*2, N_LOG_LINEAR*2);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        DrawLogFrequencyText(10.0f, "10", viewport[2], viewport[3]);
        DrawLogFrequencyText(100.0f, "100", viewport[2], viewport[3]);
        DrawLogFrequencyText(1000.0f, "1k", viewport[2], viewport[3]);
        DrawLogFrequencyText(10000.0f, "10k", viewport[2], viewport[3]);
    }
}

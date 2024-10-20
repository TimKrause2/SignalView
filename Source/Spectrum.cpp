/*
  ==============================================================================

    Spectrum.cpp
    Created: 18 Jan 2024 8:50:34pm
    Author:  tim

  ==============================================================================
*/

#include "Spectrum.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <glm/gtx/color_space.hpp>

Spectrum::Spectrum(int Nfft, double fsamplerate, int Ncopy):
    Nfft(Nfft),
    fsamplerate(fsamplerate),
    Ncopy(Ncopy)
{
    Npoints = Nfft/2 + 1;
    x_fft.reset(new double[Nfft]);
    X_fft.reset(new std::complex<double>[Npoints]);
    X_db_l.reset(new float[Npoints]);
    X_db_r.reset(new float[Npoints]);
    x_plan = fftw_plan_dft_r2c_1d(
        Nfft,
        x_fft.get(),
        reinterpret_cast<fftw_complex*>(X_fft.get()),
        FFTW_MEASURE);
    dataReady = false;
    x_cyclic_in_l.reset(new float[Nfft]);
    x_cyclic_in_r.reset(new float[Nfft]);
    x_in_l.reset(new std::unique_ptr<float[]>[Ncopy]);
    x_in_r.reset(new std::unique_ptr<float[]>[Ncopy]);
    for(int c=0;c<Ncopy;c++){
        x_in_l[c].reset(new float[Nfft]);
        x_in_r[c].reset(new float[Nfft]);
    }
    SetColors(30.0f);
    index_last=0;
    i_buffer = 0;
    i_sample = 0;
    Ncount = Nfft/Ncopy;
    count = Ncount;
}
    
Spectrum::~Spectrum()
{
}

void Spectrum::GLInit(void)
{
    tgraph.reset(new LGraph(Nfft));
    tgraph->SetLineWidths(3.0f, 1.0f);
    tgraph->SetLimits(1.0f, -1.0f);
    
    lgraph.reset(new LGraph(Npoints));
    lgraph->SetLineWidths( 3.0f, 1.0f );
    lgraph->SetLimits(0.0f, -180.0f);
    
    waterfall.reset(new Waterfall(Npoints, 128));
}

void Spectrum::GLDestroy(void)
{
    lgraph.reset(nullptr);
    tgraph.reset(nullptr);
    waterfall.reset(nullptr);
}

double window_func(double alpha)
{
    if(alpha>0.5)
        return 0.0;
    if(alpha<-0.5)
        return 0.0;
    return 0.53836 + 0.46164*cos(2.0*M_PI*(alpha-0.5));
}

double Blackman_Harris_window_func(double alpha)
{
    double a0 = 0.35875;
    double a1 = 0.48829;
    double a2 = 0.14128;
    double a3 = 0.01168;
    return a0 - a1*cos(2*M_PI*alpha) + a2*cos(4*M_PI*alpha)
           - a3*cos(6*M_PI*alpha);
}

void Spectrum::ComputeSpectrum(float *x, std::unique_ptr<float[]> &X_db)
{
    for(int i=0;i<Nfft;i++){
        double alpha = (double)i/Nfft;
        x_fft[i] = x[i]*Blackman_Harris_window_func(alpha);
    }
    fftw_execute( x_plan );
    for(int i=0;i<Npoints;i++){
        float abs_X = (float)abs(X_fft[i])/Nfft;
        //if(i) abs_X*=(float)i*100.0f/Npoints;
        if(abs_X < 1e-9) abs_X = 1e-9;
        X_db[i] = 20.0f * log10f(abs_X);
    }
}

void Spectrum::Render(void)
{
    int n = Ncopy;
    float *x_l=nullptr;
    float *x_r=nullptr;
    while(ptrFifo.GetNumReady()>0 && n!=0){
        index_last = ptrFifo.Pop();
        x_l = x_in_l[index_last].get();
        x_r = x_in_r[index_last].get();
        ComputeSpectrum(x_l, X_db_l);
        ComputeSpectrum(x_r, X_db_r);
        waterfall->InsertLine(X_db_l.get(), X_db_r.get());
        n--;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 2*viewport[3]/3, viewport[2], viewport[3]/3);
    tgraph->SetColors(color_l0, color_l1);
    tgraph->Draw(x_in_l[index_last].get());
    tgraph->SetColors(color_r0, color_r1);
    tgraph->Draw(x_in_r[index_last].get());
    
    glViewport(0, viewport[3]/3, viewport[2], viewport[3]/3);
    lgraph->SetColors(color_l0, color_l1);
    lgraph->Draw(X_db_l.get());
    lgraph->SetColors(color_r0, color_r1);
    lgraph->Draw(X_db_r.get());
    
    glDisable(GL_BLEND);
    
    glViewport(0, 0, viewport[2], viewport[3]/3);
    waterfall->Render(color_l1, color_r1);
    //std::cout << ".";
    //std::cout.flush();
}

void Spectrum::SetdBLimits(float dB_min, float dB_max)
{
    if(lgraph)
        lgraph->SetLimits(dB_max, dB_min);
    if(waterfall)
        waterfall->SetdBLimits(dB_min, dB_max);
}

void Spectrum::SetWidth(float width)
{
    if(lgraph)
        lgraph->SetViewWidth(width);
    if(waterfall)
        waterfall->SetViewWidth(width);
}

void Spectrum::EvaluateSample(float x_l, float x_r)
{
    x_cyclic_in_l[i_sample] = x_l;
    x_cyclic_in_r[i_sample] = x_r;
    i_sample++;
    if(i_sample==Nfft)
        i_sample = 0;
    
    if(--count==0){
        count = Ncount;
        int N1 = Nfft - i_sample;
        int N2 = Nfft - N1;
        int i_src = i_sample;
        int i_dst = 0;
        for(int i=0;i<N1;i++){
            x_in_l[i_buffer][i_dst] = x_cyclic_in_l[i_src];
            x_in_r[i_buffer][i_dst] = x_cyclic_in_r[i_src];
            i_src++;
            i_dst++;
        }
        i_src = 0;
        for(int i=0;i<N2;i++){
            x_in_l[i_buffer][i_dst] = x_cyclic_in_l[i_src];
            x_in_r[i_buffer][i_dst] = x_cyclic_in_r[i_src];
            i_src++;
            i_dst++;
        }
        ptrFifo.Push(i_buffer);
        i_buffer++;
        if(i_buffer==Ncopy)
            i_buffer=0;
    }
}

void Spectrum::SetColors(float hue_l)
{
    float hue_r = hue_l + 180.0f;
    if(hue_r>=360.0f)
        hue_r -= 360.0f;
    glm::vec3 hsv_l0(hue_l, 1.0f, 0.125f);
    glm::vec3 hsv_l1(hue_l, 1.0f, 1.0f);
    glm::vec3 hsv_r0(hue_r, 1.0f, 0.125f);
    glm::vec3 hsv_r1(hue_r, 1.0f, 1.0f);
    glm::vec3 rgb_l0 = glm::rgbColor(hsv_l0);
    glm::vec3 rgb_l1 = glm::rgbColor(hsv_l1);
    glm::vec3 rgb_r0 = glm::rgbColor(hsv_r0);
    glm::vec3 rgb_r1 = glm::rgbColor(hsv_r1);
    color_l0 = glm::vec4(rgb_l0, 1.0f);
    color_l1 = glm::vec4(rgb_l1, 1.0f);
    color_r0 = glm::vec4(rgb_r0, 1.0f);
    color_r1 = glm::vec4(rgb_r1, 1.0f);
}


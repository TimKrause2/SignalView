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

Spectrum::Spectrum(int Nfft, double fsamplerate, int Ncopy):
    Nfft(Nfft),
    fsamplerate(fsamplerate),
    Ncopy(Ncopy)
{
    Npoints = Nfft/2 + 1;
    x_fft.reset(new double[Nfft]);
    X_fft.reset(new std::complex<double>[Npoints]);
    X_db.reset(new float[Npoints]);
    x_plan = fftw_plan_dft_r2c_1d(
        Nfft,
        x_fft.get(),
        reinterpret_cast<fftw_complex*>(X_fft.get()),
        FFTW_MEASURE);
    dataReady = false;
    index.reset(new int[Ncopy]);
    x_in.reset(new std::unique_ptr<double[]>[Ncopy]);
    for(int c=0;c<Ncopy;c++){
        x_in[c].reset(new double[Nfft]);
        index[c] = c*Nfft/Ncopy;
    }
    Nframe = 0;
}
    
Spectrum::~Spectrum()
{
}

void Spectrum::GLInit(void)
{
    lgraph.reset(new LGraph(Npoints));
    lgraph->SetLineWidths( 3.0f, 1.0f );
    glm::vec4 color0(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec4 color1(1.0f, 1.0f, 1.0f, 1.0f);
    lgraph->SetColors(color0, color1);
    lgraph->SetLimits(0.0f, -180.0f);
    
    waterfall.reset(new Waterfall(Npoints, 128));
}

void Spectrum::GLDestroy(void)
{
    lgraph.reset(nullptr);
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


void Spectrum::Render(void)
{
    int n = Ncopy;
    while(ptrFifo.GetNumReady()>0 && n!=0){
        double *x = ptrFifo.Pop();
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
        waterfall->InsertLine(X_db.get());
        n--;
    }
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, viewport[3]/2, viewport[2], viewport[3]/2);
    //if(Nframe){
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    //    Nframe=0;
    //}else{
    //    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //    Nframe=1;
    //}
    
    glClear(GL_COLOR_BUFFER_BIT);
    lgraph->Draw(X_db.get());
    
    glViewport(0, 0, viewport[2], viewport[3]/2);
    waterfall->Render();
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

void Spectrum::EvaluateSample(float x)
{
    for(int c = 0; c<Ncopy; c++)
    {
        x_in[c][index[c]] = x;
        index[c]++;
        if(index[c]==Nfft){
            ptrFifo.Push(x_in[c].get());
            index[c]=0;
        }
    }
}

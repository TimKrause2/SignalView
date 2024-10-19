/*
  ==============================================================================

    Spectrum.h
    Created: 18 Jan 2024 8:50:34pm
    Author:  tim

  ==============================================================================
*/

#pragma once

#include <complex>
#include <fftw3.h>
#include <memory>
#include <iostream>
#include "LGraph.h"
#include "Waterfall.h"

struct PtrFifo
{
private:
    juce::AbstractFifo absFifo{ 16 };
    double *buffer[16];
public:
    void Push(double *ptr)
    {
        const auto scope = absFifo.write(1);
        if(scope.blockSize1>0)
            buffer[scope.startIndex1] = ptr;
        else
            std::cout << "PtrFifo::Push buffer full" << std::endl;
    }
    
    double *Pop(void)
    {
        double *r;
        const auto scope = absFifo.read(1);
        if(scope.blockSize1>0)
            r = buffer[scope.startIndex1];
        else
            std::cout << "PtrFifo::Pop buffer empty" << std::endl;
        return r;
    }

    int GetNumReady(void)
    {
        return absFifo.getNumReady();
    }
};

class Spectrum
{
public:
    Spectrum(int Nfft, double fsamplerate, int Ncopy);
    ~Spectrum();
    
    void GLInit(void);
    void GLDestroy(void);
    void Render(void);
    void EvaluateSample(float x);
    void SetdBLimits(float dB_min, float dB_max);
    void SetWidth(float width);
    
    
private:
    int Nfft;
    int Npoints;
    int Ncopy;
    int Nframe;
    double fsamplerate;
    std::unique_ptr<int[]> index;
    std::unique_ptr<std::unique_ptr<double[]>[]> x_in;
    std::unique_ptr<double[]> x_fft;
    bool dataReady;
    std::unique_ptr<std::complex<double>[]> X_fft;
    fftw_plan x_plan;
    std::unique_ptr<float[]> X_db;
    PtrFifo ptrFifo;
    
    std::unique_ptr<LGraph> lgraph;
    std::unique_ptr<Waterfall> waterfall;
};



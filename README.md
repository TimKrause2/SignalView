# SignalView

## Introduction

Signal View is a stereo signal visual analyser.
It includes a time domain view and frequency domain view in selectable
linear or log format. There is also a waterfall view to see trends in
the spectrum.

## Implementation

This project uses [Juce](https://juce.com/) and it's OpenGL hosting capabilities.
The spectral analysis is performed by [fftw3](https://www.fftw.org/).

## Building for Linux

To build for Linux in the terminal:

    $ ./build_linux.sh

This will build the VST3 bundle and also copy it into your local
VST3 directory.

## Building for Windows



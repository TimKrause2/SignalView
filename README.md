# SignalView

## Introduction

Signal View is a stereo signal visual analyser.
It includes a time domain view and frequency domain view in selectable
linear or log format. There is also a waterfall view to see trends in
the spectrum.

## Download the plugin

The plugin is available for download [here](https://www.twkrause.ca).

## Implementation

This project uses [Juce](https://juce.com/) and it's OpenGL hosting capabilities.
The spectral analysis is performed by [fftw3](https://www.fftw.org/).

## Building prerequisits for Linux

- curl
- make
- gcc
- Juce with Projucer

## Building prerequisits for Windows

- Visual Studio 2022
- Juce with Projucer

## Building for Linux

Open the project file, `SignalView.jucer`, in Projucer. Select File->SaveProject.
And then in the terminal in the `SignalView` directory type the following:

    $ ./build_linux.sh

This will build the VST3 bundle and also copy it into your local
VST3 directory.

## Building for Windows



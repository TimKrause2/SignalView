# SignalView

![SignalView screen shot.](<https://www.twkrause.ca/SignalView screenshot.png> "SignalView screen shot")

## Introduction

Signal View is a stereo signal visual analyser VST3 plugin.
It includes a time domain view and frequency domain view in selectable
linear or log format. There is also a waterfall(spectrogram) view to see trends in
the spectrum.

## Download the plugin

The plugin is available for download [here](https://www.twkrause.ca).

## Implementation

This project uses [Juce and Projucer](https://juce.com/) and its OpenGL hosting capabilities.
The spectral analysis is performed by [fftw3](https://www.fftw.org/).
Static libraries are provided by [vcpkg](https://vcpkg.io/).

## Building prerequisits

### Linux

- [Juce with Projucer](https://juce.com/)
- curl
- make
- gcc

### Windows

- [Juce with Projucer](https://juce.com/)
- [Visual Studio 2022](https://visualstudio.microsoft.com/)

## Building for Linux

1. Open the project file, `SignalView.jucer`, in Projucer.
2. Select **File->SaveProject**.
3. Then in the terminal in the `SignalView` directory enter the following:

        ./linux_build.sh

   This will build the VST3 bundle and also copy it into your local VST3 directory.

## Building for Windows

1. Open the project file, `SignalView.jucer`, in Projucer.
2. Select Visual Studio 2022 in the **Select exporter** entry at the top.
3. Click on the Visual Studio icon to the right. This will open Visual Studio.
4. Once in Visual Studio select **Tools->Command Line->Developer Command Prompt**.
5. In the terminal enter the following:
   
        ..\..\windows_pre_build.bat
   
7. After that completes select **Build** in Visual Studio.
8. Now the plugin is in either
   `SignalView\Builds\VisualStudio2022\x64\Debug\VST3` or
   `SignalView\Builds\VisualStudio2022\x64\Release\VST3`
   depending on the target you selected.
   That can then be copied into `C:\Program Files\Common Files\VST3` to make it accessible to your host.

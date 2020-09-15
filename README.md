# Phase Rotation Experiment
Testing out FFT phase manipulation for phase rotation in audio signals.

![Screenshot](screenshot.png.png "Screenshot")

Currently tested phase rotation using FFTs on 
- sinewaves
- noise
- squarewaves
- sawtoothwaves 
- a mixture of above. 

To easily manipulate audio signals I've used the JUCE framework. Not much time has been spent on extensibility or performance. 
Binaries of the applicaion can be found inside the ```Builds``` folder.

## Features
- Waveform selection
- FFT size selection
- Visual feedback on timedomain and frequency domain
- UI visual preference settings

## Purpose
In order to develop a better understanding of phase and magnitude relations to time domain signals, 
I wanted to gain some simple intuitive insight in how the frequency domain & time domain influence eachother. 
This software is designed to gain insight on relations such us:
- Timedomain transients and frequency domain phase
- Non circular (frequencies in between bins) fft input and frequency response

## Possible future features
- Waveform, phase and magnitude editing
- Real-imaginary plot

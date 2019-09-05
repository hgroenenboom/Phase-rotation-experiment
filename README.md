# phase-rotation-test
Testing out FFT phase manipulation for phase rotation in audio signals.

Currently tested phase rotation using FFTs on sinewaves, noise, squarewaves, sawtoothwaves and a mixture of those. To easily manipulate audio signals I've used the JUCE framework. Not much time has been spent on extensibility or performance. Builds of the first test can be found inside the "Builds" folder.

## Features
- Flexibel interface
- Visual feedback on timedomain and frequency domain
- UI visual preference settings

## Purpose
In order to develop a better understanding of phase and magnitude relations to time domain signals, I wanted to gain some simple intuitive insight in how the frequency domain & time domain influence eachother. This software is designed to gain insight on relations such us:
- Timedomain transients and frequency domain phase
- Non circular (frequencies in between bins) fft input and frequency response

## Possible future features
- Waveform, phase and magnitude editing
- Real-imaginary plot

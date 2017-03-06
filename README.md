# MusicSequencerTest
Multi-track music sequencer using the Texas Instruments SN76849 sound chip
===============================================================================

This quick-and-dirty example allows sequencing multitrack music on the SN76849 sound chip. It is made up of three components plus some support routines:

1. SN76849 "driver" - This has routines to poke bytes out to the chip, and some functions that use this routine to start a tone and adjust the volume of it. It works by mapping note values (0-87) to match the 88 piano keys. There is a frequency table matching the values when using a 4mhz crystal. There is no real support for the fourth white noise channel yet, but that will be coming soon. (This is basically my first pass at the code.) There is also a routine which fades out the notes, simulating decay. It will eventually become an ADSR feature so tones can do more than just beep or fade out.
2. MusicSequencer - This is a simple track sequencer where you can place notes (using defines for the note value and defines for the length) like sheet music and then have the tune played. It's very simple right now. It has a handler function that gets called repeatedly in a loop to handle the actual sequencing.
3. MusicSequencerTest - A simple program that uses the above two items to playt he Pac-Man intro music. Sort of.
4. LineInput - A simple line input routine, that has a hook inside to call the tone handler so it can keep processing tones while waiting for input. Quick and dirty. I'll be fixing this up soon, too.

It was built with the current 1.8.1 version of the Arduino IDE. I'll be adding a few more things to it, as well as white noise support (and some form of drum track method), as soon as I have a moment.

REVISION
========
* 2017-03-03 allenh - Initial, hastily created README file.
* 2017-03-05 achuff - Added notes about what the three main compoents are and what they do.

FILES
=====

* README.md - this file
* LineInput.ino - simple line input routine that calls the player while waiting for input.
* MusicSequencer.h
* MusicSequencer.ino
* MusicSequencerTest.ino - test program that plays the 2-voice Pac-Man intro music.
* SN76849.ino - code to send commands to the Texas Instrument SN76849 sound chip.
* SN76849.h

CONFIGURATION
=============

Edit the SN76849.h as appropriate:

```
/*      Chip Pin        Arduino Pin */
#define SN76489_D0      2  //D2
#define SN76489_D1      3  //D3
#define SN76489_D2      4  //D4
#define SN76489_D3      6  //D6
#define SN76489_D4      7  //D7
#define SN76489_D5      12 //D12
#define SN76489_D6      11 //D11
#define SN76489_D7      10 //D10

#define SN76489_WE      5  //D5
```

 Notes are in the source code, until I have time to update this README.

RUNNING
=======
 
 More to come...

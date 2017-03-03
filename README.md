# MusicSequencerTest
Multi-track music sequencer using the Texas Instruments SN76849 sound chip

===============================================================================

...

REVISION
========
* 2017-03-03 allenh - Initial, hastily created README file.

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

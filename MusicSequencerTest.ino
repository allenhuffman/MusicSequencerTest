/*---------------------------------------------------------------------------*/
/*
  Music Sequencer Test program
  By Allen C. Huffman
  www.subethasoftware.com

  Simple program to play the Pac-Man intro tune.

  REFERENCES:

  Pac-Man tune from this sheet music:

  https://musescore.com/user/85429/scores/107109

  CONFIGURATION:
  
  VERSION HISTORY:
  2017-03-01 0.0 allenh - In the beginning...
  2017-03-03 0.1 allenh - Fixing chip name typos.
  2017-03-06 0.2 allenh - Using END define. Comments.

  TODO:
  * Move music data to PROGMEM.
  * Make "interactive tone demo" do channel,note,volume.
  
  TOFIX:
  
*/
/*---------------------------------------------------------------------------*/
#define VERSION "0.2"

#include "SN76489.h"        // for NOTES, etc.

#include "MusicSequencer.h" // for MusicStruct, sequencer*()

/* Song */
/* 0 - Track 0
   1 - Track 1
   2 - Track 3
   3 - Track 4 Noise (percussion)
*/

#define TRACKS 2  // Tracks in this sequence.

// TODO: MAKE THIS PROGMEM!!!
MusicStruct g_track0[] = // Bass
{
  { NB2, L8DOTTED },
  { NB3, L16 },
  { NB2, L8DOTTED },
  { NB3, L16 },

  { NC3, L8DOTTED },
  { NC4, L16 },
  { NC3, L8DOTTED },
  { NC4, L16 },

  { NB2, L8DOTTED },
  { NB3, L16 },
  { NB2, L8DOTTED },
  { NB3, L16 },

  { NF3S, L8 },
  { NG3S, L8 },
  { NA3S, L8 },
  { NB3, L8 },

  { END, END } // END
};

MusicStruct g_track1[] =
{
  { NB3, L16 },
  { NB4, L16 },
  { NF4S, L16 },
  { ND4S, L16 },
  { NB4, L32 },
  { NF4S, L16DOTTED },
  { ND4S, L8 },

  { NC4, L16 },
  { NC5, L16 },
  { NG4, L16 },
  { NE4, L16 },
  { NC5, L32 },
  { NG4, L16DOTTED },
  { NE4, L8 },

  { NB3, L16 },
  { NB4, L16 },
  { NF4S, L16 },
  { ND4S, L16 },
  { NB4, L32 },
  { NF4S, L16DOTTED },
  { ND4S, L8 },

  { ND4S, L32 },
  { NE4, L32 },
  { NF4S, L16 },

  { NF4S, L32 },
  { NG4, L32 },
  { NG4S, L16 },

  { NG4S, L32 },
  { NA4, L32 },
  { NA4S, L16 },
  { NB4, L8 },

  { END, END }
};

MusicStruct *g_pacManIntro[TRACKS] = { g_track0, g_track1 };

#define TX_PIN          7
#define RX_PIN          8
#define LED_PIN         11

void setup()
{
  Serial.begin(9600);
  Serial.println(F("SN76489 Test Program "VERSION));

  initSN76489();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Notes about using an Arduino pin to generate the 4Mhz pulse:

  // For the Teensy 2.0, this is how to make a pin act as a 4MHz pulse.
  // I am using this on my Teensy 2.0 hardware for testing. This can be
  // done on other Arduino models, too, but I have only been using my
  // Teensy for this so far. My original NANO prototype is using an
  // external crystal.

  //pinMode(14, OUTPUT); 
  // Turn on toggle pin mode.
  //TCCR1A |= ((1<<COM1A1));
  // Set CTC mode (mode 4), and set clock to be CPU clock
  //TCCR1B |= ((1<<WGM12) | (1<<CS10));
  // Count to one and then reset and count again.  Since CPU is 16MHz,
  // this will divide clock by 2 and action on the pin.  Since we will be
  // toggling the pin, that will divide by 2 again, giving /4 or 4MHz
  //OCR1A = 1;

  // Make pin 14 be a 14Mhz signal on the Teensy 2.0:
#ifdef TEENSY20
  pinMode(14,OUTPUT);
  TCCR1A = 0x43;
  TCCR1B = 0x19;
  OCR1A = 1;
#endif

  setMaxVolume( 0 ); // 0=high, 15=silent

  muteAll(); // Just in case...
} // end of setup()

void loop()
{
  bool isPlaying;

  // Sequence test.
  isPlaying = sequencerStart(g_pacManIntro, TRACKS, 15); // TEMPO

  while( isPlaying == true )
  {
    isPlaying = sequencerHandler();
  };

  muteAll();

  // Interactive tone demo.
  while (1) {
    char  buffer[10];
    byte  channel;
    byte  note;
    byte  volume;

    //Serial.print(F("channel (0-3), note (1-88), volume (0-15) :"));
    Serial.print(F("note (0-87) :"));

    lineinput(buffer, sizeof(buffer));
    note = atoi(buffer);
    channel = 0;
    volume = 0; // LOUD

    Serial.print(F("play("));
    Serial.print(channel);
    Serial.print(F(", "));
    Serial.print(note);
    Serial.print(F(", "));
    Serial.print(volume);
    Serial.println(F(")"));

    play(0, atoi(buffer));
    // delay(100);
    // play(0, 0);
  }

  Serial.println("Off...");
  muteAll();
} // end of loop()

/*---------------------------------------------------------------------------*/
// End of MusicSequencerTest


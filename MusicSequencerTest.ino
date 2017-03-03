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

  TODO:
  
  TOFIX:
  
*/
/*---------------------------------------------------------------------------*/

#include "SN76849.h"        // for NOTES, etc.

#include "MusicSequencer.h" // for MusicStruct, sequencer*()

/* Song */
/* 0 - Track 0
   1 - Track 1
   2 - Track 3
   3 - Track 4 Noise (percussion)
*/

#define TRACKS 2

// TODO: MAKE THIS PROGMEM!!!
const MusicStruct g_track0[] = // Bass
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

  { 0, 0 } // END
};

const MusicStruct g_track1[] =
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

  { 0, 0 }
};

const MusicStruct *g_pacManIntro[TRACKS] = { g_track0, g_track1 };

void setup()
{
  Serial.begin(9600);

  initSN76489();

  muteAll(); // Just in case...

  Serial.println(F("SN76489 Test Program"));
}

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

    Serial.print(F("channel (0-3), note (1-88), volume (0-15) :"));

    lineinput(buffer, sizeof(buffer));

    Serial.print(F("play("));
    Serial.print(channel);
    Serial.print(F(", "));
    Serial.print(note);
    Serial.print(F(", "));
    Serial.print(volume);
    Serial.println(F(")"));

    play(0, atoi(buffer));
    //delay(100);
    // play(0, 0);
  }

  Serial.println("Off...");
  muteAll();
}

/*---------------------------------------------------------------------------*/
// End of MusicSequencerTest


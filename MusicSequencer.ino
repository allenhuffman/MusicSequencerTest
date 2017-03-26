//#define DEBUG
/*-----------------------------------------------------------------------------

  Music Sequencer
  By Allen C. Huffman
  www.subethasoftware.com

  Simple music sequencer. As long as the player routine is called more often
  than the shortest notes, it should do a pretty good job at playing music.

  REFERENCES:


  CONFIGURATION:
  1. ...

  VERSION HISTORY:
  2017-03-01 0.0 allenh - In the beginning...
  2017-03-06 0.1 allenh - Using renamed playHandler() function. Comments.

  TODO:
   ...

  TOFIX:
   ...
  -----------------------------------------------------------------------------*/
//#define VERSION "0.1"

#include "MusicSequencer.h"

/*---------------------------------------------------------------------------*/
// STATIC GLOBALS
/*---------------------------------------------------------------------------*/

static int            S_tempo = 25;  // Master tempo multiplier.
static bool           S_started = false; // Playback started?

static MusicStruct    **S_sequence; // Points to sequence structure.
static byte           S_tracks;     // Number of tracks in sequence.

/*---------------------------------------------------------------------------*/
// FUNCTIONS
/*---------------------------------------------------------------------------*/

/*
 * Start the specified sequence.
 * 
 * *sequence[] - pointer to sequence structure
 * 
 * tracks - how many tracks it contains
 * 
 * tempo - tempo multiplier (1=fast, XX=slower)
 */
bool  sequencerStart( MusicStruct *sequence[], byte tracks, byte tempo)
{
  bool status;
  
  // TODO: Error checking would be nice here.
  if (sequence == NULL)
  {
#if defined(DEBUG)
    Serial.println(F("sequencerStart: sequence == NULL"));
#endif

    status = false;  
  }
  else if (tracks >= MAX_TRACKS)
  {
#if defined(DEBUG)
    Serial.println(F("sequencerStart: tracks > MAX_TRACKS"));
#endif

    status = false;
  }
  else
  {
    S_started = false;
    S_sequence = sequence;
    S_tracks = tracks;
    S_tempo = tempo;
  
#if defined(DEBUG)
    Serial.print(F("sequencerStart: "));
    Serial.print((unsigned long)sequence, HEX);
    Serial.print(F(" "));
    Serial.println(tracks, DEC);
#endif

    status = true;
  }

  return status;
} // end of sequencerStart()

/*---------------------------------------------------------------------------*/

/*
 * Sequencer handler routine. This must be called repeatedly so it can do
 * timing and switch to the next note.
 */
bool sequencerHandler()
{
  //bool    status = false;
  int     i;
  uint8_t note, duration;

  static unsigned long  s_playNextTime[MAX_TRACKS];
  static int   s_currentNote[MAX_TRACKS];
  static int            s_tracksPlaying = 0;
  
  if (S_started == false)
  {
    // Initialize
    for (i = 0; i < S_tracks; i++)
    {
      s_playNextTime[i] = millis();
      s_currentNote[i] = 0;
    }

    S_started = true;
    s_tracksPlaying = S_tracks;
  }

  // Loop through note data looking for something to play.
  for (i = 0; i < S_tracks; i++)
  {
    // Skip if already done.
    if (s_currentNote[i] == -1)
    {
      continue;
    }

    //if ( (long)(millis()-s_playNextTime[i] >=0 ) )
    if ( (long)(millis() > s_playNextTime[i]) )
    {
      
      int currentNote = s_currentNote[i];

      note = S_sequence[i][currentNote].note;
      duration = S_sequence[i][currentNote].duration;

      // If we read END for both, that track is done.
      if ( (note == END) && (duration == END) )
      {
#if defined(DEBUG)
        Serial.print(F("Track "));
        Serial.print(i);
        Serial.println(F(" out of data."));
#endif        
        s_currentNote[i] = -1;
        s_tracksPlaying--; // One less track is playing.
        break;
      }

#if defined(DEBUG)
      Serial.print("V");
      Serial.print(i, DEC);
      Serial.print(":");
      Serial.print(note);
      Serial.print(",");
      Serial.print(duration);
      Serial.print(" ");
#endif

      play(i, note );

      s_playNextTime[i] = s_playNextTime[i] + (duration * S_tempo);

#if defined(DEBUG)
      Serial.print(" (time: ");
      Serial.print(s_playNextTime[i]);
      Serial.print(") ");
#endif

      s_currentNote[i]++;
    }
  } // end of for (i = 0; i < S_tracks; i++)

#if defined(DEBUG)
  if ( (note != 0) && (duration != 0))
  {
    Serial.println();
  }
#endif

  // Handle note play stuff (ADSR, volume decay, etc.).
  playHandler();

  // true means tracks are playing, false means they are not.
  return (s_tracksPlaying==0 ? false : true);
} // end of sequencerHandler()

/*---------------------------------------------------------------------------*/
// End of MusicSequencer


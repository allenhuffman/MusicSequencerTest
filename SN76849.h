#ifdef SN76849_H
#define SN76849_H
/*---------------------------------------------------------------------------*/
/*
  SN76489 sound chip routines
  By Allen C. Huffman
  www.subethasoftware.com

  Header file with chip definitions.

  VERSION HISTORY:
  2017-03-01 0.0 allenh - In the beginning...

  TODO:
  1. Note 0 is used for off, but since we can't play anything that low
     anyway, it's not a big deal. But, it would be nice to make the notes
     be 1-88 (base 1) instead of 0-87 (base 0).

  TOFIX:

*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
// EXTERNAL PROTOTYPES
/*---------------------------------------------------------------------------*/
void initSN76849();

// API:
void play(byte channel, uint16_t note);
void volume( byte channel, byte volume );

void decayHandler();

// Convienience functions.
void muteAll();

#endif // SN7684_H

/*---------------------------------------------------------------------------*/
// End of SN76849.h


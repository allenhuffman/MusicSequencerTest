//#define DEBUG
/*---------------------------------------------------------------------------*/
/*
  SN76489 sound chip routines
  By Allen C. Huffman
  www.subethasoftware.com

  Some simple functions to making this sound chip beep.

  REFERENCES:

  Information on the SN76849 came from this article:

  http://danceswithferrets.org/geekblog/?p=93

  And these links found in it:

  http://www.smspower.org/Development/SN76489
  http://members.casema.nl/hhaydn/howel/parts/76489.htm

  CONFIGURATION:
  1. Nine (9) digital out pins must be defined.

  VERSION HISTORY:
  2017-03-01 0.0 allenh - In the beginning...

  TODO:
   Add ADSR support.

  TOFIX:
   TODO...
*/
/*---------------------------------------------------------------------------*/
#define VERSION "0.0"

#include "SN76849.h"

/*---------------------------------------------------------------------------*/
// CONFIGURATION
/*---------------------------------------------------------------------------*/
//
// The SN76489 uses 8 digital output lines to set the bits in a byte, then
// uses another digital out to toggle HIGH/LOW to set that byte.
//

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

#define SN76489_CE      // Chip Enable - Not used here.
#define SN76489_READY   // Not used here.

/*
   D5  ->  1 +-----+ 16 <- VCC
   D6  ->  2 | S N | 15 <- D4
   D7  ->  3 |  7  | 14 <- CLK
   RDY ->  4 |  6  | 13 <- D3
   !WE ->  5 |  4  | 12 <- D2
   !CE ->  6 |  8  | 11 <- D1
   AUD ->  7 |  9  | 10 <- D0
   GND -?  8 +-----+ 9  <- NC

  D0-D7 go to Arduino digital outputs.
  WE goes to a Arduino digital output.
  VCC goes to 5V.
  GND goes to ground.
  AUD goes to a headphone jack (other side to GND?).
  CLK goes to a 4MHz crystal (which is hooked to 5V and GND).
  CE goes to 5V (always enabled; but it could be tied to a pin for using
      more than one device on those lines).
  RDY is not used.
*/

/*---------------------------------------------------------------------------*/
// INTERNAL DEFINES
/*---------------------------------------------------------------------------*/
// 1CCTDDDD - 1=Latch+Data, CC=Channel, T=Type, DDDD=Data1
#define LATCH_CMD 0b10000000
#define CHANNEL0  0b00000000
#define CHANNEL1  0b00100000
#define CHANNEL2  0b01000000
#define CHANNEL3  0b01100000
#define TYPE_VOL  0b00010000
#define VOL_OFF   0b00001111 // 15
#define VOL_LOW   0b00001010 // 10
#define VOL_MED   0b00000101 // 5
#define VOL_MAX   0b00000000 // 0
#define VOL_DEC   +1
#define VOL_INC   -1
#define TYPE_TONE 0b00000000
#define DATA1     0b00001111 // LSB of data [DATA2|DATA1]
#define NOISE_MODE 0b0000100 // 0=white, 1=periodic
#define NOISE_RATE 0b0000011 //

// 0XDDDDDD - 0=Data, X=Ignored, DDDDDD = Data2
#define DATA_CMD  0b00000000
#define DATA2     0b00111111 // MSB of data (if needed)

#define DECAYTIME_MS 50

// These defines make us ignore values sent too low or too high.
#define LOWEST_NOTE   0  //NB2
#define HIGHEST_NOTE  88 //NC8 // It is really NC9, beyond piano.

/*---------------------------------------------------------------------------*/
// STATIC GLOBALS
/*---------------------------------------------------------------------------*/
static byte     S_volume = VOL_MAX;  // Default maximum volume.

/* Default to off. */
static uint8_t  S_vol[4] = { VOL_OFF, VOL_OFF, VOL_OFF, VOL_OFF };

// SN76489 note table.
// This table defines the values used to represent all 88 notes of a piano.
// 4Mhz Crystal
/* ___________________________________________      _______________________
   # # | # # | # # # | # # | # # # | # # | # #......# # # | # # | # # # | #
   # # | # # | # # # | # # | # # # | # # | # #......# # # | # # | # # # | #
   |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|......|_|_|_|_|_|_|_|_|_|_|_|
    A B C D E F G A B C D E F G A B C D E F G        G A B C D E F G A B C 
    0 0 1 1 1 1 1 1 1 2 2 2 2 2 2 2 3 3 3 3 3        6 6 6 7 7 7 7 7 7 7 8 
*/

static const uint16_t PROGMEM G_notes[88] = {
  4545,  // 0x11C1 - A0  - Lowest piano key.
  4290,  // 0x10C2 - A0#
  4050,  // 0xFD2  - B0

  3822,  // 0xEEE  - C1
  3608,  // 0xE18  - C1#
  3405,  // 0xD4D  - D1
  3214,  // 0xC8E  - D1#
  3034,  // 0xBDA  - E1
  2863,  // 0xB2F  - F1
  2703,  // 0xA8F  - F1#
  2551,  // 0x9F7  - G1
  2408,  // 0x968  - G1#
  2273,  // 0x8E1  - A1
  2145,  // 0x861  - A1#
  2025,  // 0x7E9  - B1

  1911,  // 0x777  - C2
  1804,  // 0x70C  - C2#
  1703,  // 0x6A7  - D2
  1607,  // 0x647  - D2#
  1517,  // 0x5ED  - E2
  1432,  // 0x598  - F2
  1351,  // 0x547  - F2#
  1276,  // 0x4FC  - G2
  1204,  // 0x4B4  - G2#
  1136,  // 0x470  - A2
  1073,  // 0x431  - A2#
  1012,  // 0x3F4  - B2  - LOWEST NOTE AT 4Mhz? (26)

  956,   // 0x3BC  - C3
  902,   // 0x386  - C3#
  851,   // 0x353  - D3
  804,   // 0x324  - D3#
  758,   // 0x2F6  - E3
  716,   // 0x2CC  - F3
  676,   // 0x2A4  - F3#
  638,   // 0x27E  - G3
  602,   // 0x25A  - G3#
  568,   // 0x238  - A3
  536,   // 0x218  - A3#
  506,   // 0x1FA  - B3

  478,   // 0x1DE  - C4
  451,   // 0x1C3  - C4#
  426,   // 0x1AA  - D4
  402,   // 0x192  - D4#
  379,   // 0x17B  - E4
  358,   // 0x166  - F4
  338,   // 0x152  - F4#
  319,   // 0x13F  - G4
  301,   // 0x12D  - G4#
  284,   // 0x11C  - A4  - 440hz
  268,   // 0x10C  - A4#
  253,   // 0xFD   - B4

  239,   // 0xEF   - C5  - Middle C
  225,   // 0xE1   - C5#
  213,   // 0xD5   - D5
  201,   // 0xC9   - D5#
  190,   // 0xBE   - E5
  179,   // 0xB3   - F5
  169,   // 0xA9   - F5#
  159,   // 0x9F   - G5
  150,   // 0x96   - G5#
  142,   // 0x8E   - A5
  134,   // 0x86   - A5#
  127,   // 0x7F   - B5

  119,   // 0x77   - C6
  113,   // 0x71   - C6#
  106,   // 0x6A   - D6
  100,   // 0x64   - D6#
  95,    // 0x5F   - E6
  89,    // 0x59   - F6
  84,    // 0x54   - F6#
  80,    // 0x50   - G6
  75,    // 0x4B   - G6#
  71,    // 0x47   - A6
  67,    // 0x43   - A6#
  63,    // 0x3F   - B6

  60,    // 0x3C   - C7
  56,    // 0x38   - C7#
  53,    // 0x35   - D7
  50,    // 0x32   - D7#
  47,    // 0x2F   - E7
  45,    // 0x2D   - F7
  42,    // 0x2A   - F7#
  40,    // 0x28   - G7
  38,    // 0x26   - G7#
  36,    // 0x24   - A7
  34,    // 0x22   - A7#
  32,    // 0x20   - B7

  30,    // 0x1E   - C8  - Highest piano key.
  /*
    28,    // 0x1C   - C8#
    27,    // 0x1B   - D8
    25,    // 0x19   - D8#
    24,    // 0x18   - E8
    22,    // 0x16   - F8
    21,    // 0x15   - G8
    20,    // 0x14   - G8#
    19,    // 0x13   - A8
    18,    // 0x12   - A8#
    17,    // 0x11   - B8

    16,    // 0x10   - C9  - HIGHEST NOTE at 4MHz? (98)
    15,    // 0xF    - C8#
    14,    // 0xE    - D8
    13,    // 0xD    - D8#
    13,    // 0xD    - E8
    12,    // 0xC    - F8
    11,    // 0xB    - F8#
    11,    // 0xB    - G8
    10,    // 0xA    - G8#
  */
};

/*---------------------------------------------------------------------------*/
// INTERNAL PROTOTYPES
/*---------------------------------------------------------------------------*/
void poke(uint8_t);

/*---------------------------------------------------------------------------*/
// EXTERNAL FUNCTIONS
/*---------------------------------------------------------------------------*/

/*
 * Initialize pins to be outputs.
 * Silence all channels.
 */
void initSN76489()
{
  /* Enable pins to be OUTPUTs */
  pinMode(SN76489_D0, OUTPUT);
  pinMode(SN76489_D1, OUTPUT);
  pinMode(SN76489_D2, OUTPUT);
  pinMode(SN76489_D3, OUTPUT);
  pinMode(SN76489_D4, OUTPUT);
  pinMode(SN76489_D5, OUTPUT);
  pinMode(SN76489_D6, OUTPUT);
  pinMode(SN76489_D7, OUTPUT);
  pinMode(SN76489_WE, OUTPUT);

  muteAll();
}

/*---------------------------------------------------------------------------*/
// PUBLIC API:
//
// play( channel, note ) - called to play a tone.
//
// playHandler() - called in main loop to handle ADSR of notes.
//
// volume( channel, volume )
//

/*
 * Play a tone on a specified channel.
 * 
 * channel  = 0-3
 * note     = 0 (off), 1-88 (piano note)
*/
void play(byte channel, uint16_t note)
{
  /* If note is in our playable range... */
  if (note < sizeof(G_notes))
  {
    /* Convert channel 0-3 to bits. */
    if (channel <= 3)
    {
      if (note == 0)
      {
        volume( channel, VOL_OFF );
        // poke( LATCH_CMD | CHANNEL0 | TYPE_VOL | VOL_OFF );
      }
      else
      {
        uint16_t freq;

        /* Quick check so we don't bother playing things we can't play. */
        if ( (note < LOWEST_NOTE) || (note > HIGHEST_NOTE) ) return;
        // TODO: Clean this up so we only have one place to return.

        /* Get the frequency from the table. */
        freq = pgm_read_word(&G_notes[note]);

        /* Shift channel value (00000011) five places to the left. */
        poke( LATCH_CMD | (channel << 5) | TYPE_TONE | (freq & 0b1111) );
        /* If more bits are used than fit in 4... */
        if (freq > 0b1111)
        {
          /* ...issue second DATA byte with the rest. */
          poke( DATA_CMD | (freq >> 4 ) );
        }

        /* Set channel to default volume. */
        volume(channel, S_volume);
        
        /* Cache the volume level so we can use it later. */
        S_vol[channel] = S_volume;

      } // end of if (note==0) else
    } // end of if (channel <= 3)
#if defined(DEBUG)
    else // not if (channel <= 3)
    {
      // Channel >3! Error.
      Serial.print(F("Invalid channel (0-3 allowed): "));
      Serial.println(channel );
    }
#endif
  } // end of if (note < sizeof(g_notes4mhz))
#if defined(DEBUG)
  else // note is higher than the table contains.
  {
    // Note >88! Error.
    Serial.print(F("Invalid note (0, 1-88 allowed): "));
    Serial.println(note);
  }
#endif
}

/*
 * Set volume of specified channel.
 * 
 * channel  = 0-3
 * volume   = VOL_OFF - VOL_MAX
 */
void volume( byte channel, byte volume )
{
  /* Convert channel 0-3 to bits. */
  if (channel <= 3)
  {
    /* Shift 00000011 five places to the left. */
    poke( LATCH_CMD | (channel << 5) | TYPE_VOL | (volume & 0b1111) );

      // Update our static.
    S_vol[channel] = volume;

  } // end of if (channel <= 3)
#if defined(DEBUG)
  else // channel is higher than 3.
  {
    // Channel >3! Error.
    Serial.print(F("Invalid channel (0-3 allowed): "));
    Serial.println(channel);
  }
#endif
}


/* Issue Volume 0 to all four channels. */
void muteAll()
{
  volume(0, VOL_OFF);
  volume(1, VOL_OFF);
  volume(2, VOL_OFF);
  volume(3, VOL_OFF);
}

// ADSR stuff will go here, eventually. But for now, just decay...
void decayHandler()
{
  static unsigned long s_decayTime = 0;
  static unsigned int  s_decayRate = 30;

  // LED blinking heartbeat. Yes, we are alive.
  if ( (long)(millis() - s_decayRate) >= s_decayTime )
  {
    // Reset "next time to toggle" time.
    s_decayTime = millis() + s_decayRate;
  }
  else
  {
    return;
  }

  /* Handle notes that might be playing. */
  for (int i = 0; i < 3; i++)
  {
    /* If voice volume is not off... */
    if (S_vol[i] != VOL_OFF)
    {
      /* Decrement volume. */
      S_vol[i] = S_vol[i] + VOL_DEC;
      volume(i, S_vol[i]);
    }
  }
}

/*
 * Send byte to SN76489.
 * 
 * b = byte to send
 */
static void poke(byte b)
{
#if defined(DEBUG)
  Serial.print(F("poke ")); Serial.print(b, BIN);
  Serial.print(F(" ")); Serial.println(b, HEX);
#endif

  digitalWrite(SN76489_WE, HIGH);

  digitalWrite(SN76489_D0, (b & bit(0)) ? HIGH : LOW);
  digitalWrite(SN76489_D1, (b & bit(1)) ? HIGH : LOW);
  digitalWrite(SN76489_D2, (b & bit(2)) ? HIGH : LOW);
  digitalWrite(SN76489_D3, (b & bit(3)) ? HIGH : LOW);
  digitalWrite(SN76489_D4, (b & bit(4)) ? HIGH : LOW);
  digitalWrite(SN76489_D5, (b & bit(5)) ? HIGH : LOW);
  digitalWrite(SN76489_D6, (b & bit(6)) ? HIGH : LOW);
  digitalWrite(SN76489_D7, (b & bit(7)) ? HIGH : LOW);

  digitalWrite(SN76489_WE, LOW);

  delay(1);

  digitalWrite(SN76489_WE, HIGH);
}

/*---------------------------------------------------------------------------*/
// End of SN76489.ino


/*---------------------------------------------------------------------------*/
/*
  Line Input routine.
  By Allen C. Huffman
  www.subethasoftware.com

  Simple line input routine, modified to call the sequencerHandler() inside
  the "waiting for a character" routine. This allows the Music Sequencer to
  play music while the user is entering input. :)

  REFERENCES:

  CONFIGURATION:

  VERSION HISTORY:
  2017-03-01 0.0 allenh - In the beginning...

  TODO:

  TOFIX:

*/
/*---------------------------------------------------------------------------*/

// Read string up to len bytes. This code comes from my Hayes AT Command
// parser, so the variables are named differently.

/*---------------------------------------------------------------------------*/
// DEFINES
/*---------------------------------------------------------------------------*/

#define CR           13
#define BEL          7
#define BS           8
//#define CAN          24

/*---------------------------------------------------------------------------*/
// FUNCTIONS
/*---------------------------------------------------------------------------*/

/*
 * Line Input routine.
 * 
 * *cmdLine - pointer to buffer to hold the input.
 * 
 * len - length of the input buffer.
 */
byte lineinput(char *cmdLine, byte len)
{
  int     ch;
  byte    cmdLen = 0;
  bool    done;

  done = false;
  while(!done)
  {
    //ledBlink();
    decayHandler();

    ch = -1; // -1 is no data available

    if (Serial.available()>0)
    {
      ch = Serial.read();
    }
    else
    {
      continue; // No data. Go back to the while()...
    }
    switch(ch)
    {
    case -1: // No data available.
      break;

    case CR:
      Serial.println();
      cmdLine[cmdLen] = '\0';
      done = true;
      break;

      /*case CAN:
       print(F("[CAN]"));
       cmdLen = 0;
       break;*/

    case BS:
      if (cmdLen>0)
      {
        Serial.print((char)BS);
        //printSemi(F(" "));
        //printCharSemi(BS);
        cmdLen--;
      }
      break;

    default:
      // If there is room, store any printable characters in the cmdline.
      if (cmdLen<len)
      {
        if ((ch>31) && (ch<127))  // isprint(ch) does not work.
        {
          Serial.print((char)ch);
          cmdLine[cmdLen] = ch;   // toupper(ch);
          cmdLen++;
        }
      }
      else
      {
        Serial.print((char)BEL); // Overflow. Ring 'dat bell.
      }
      break;
    } // end of switch(ch)
  } // end of while(!done)

  return cmdLen;
}

/*---------------------------------------------------------------------------*/
// End of LineInput


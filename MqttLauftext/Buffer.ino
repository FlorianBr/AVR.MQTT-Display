/*
 * Functions for the Buffer Handling
 */
#include "zxpix_font.h"

#define LINES   2         // Number of Lines

// Line-Buffers:
// Controller(16 Byte) * Controllers per Line (2) * Modules in Chain (2) * Blocks (2) = 128 Byte
uint8_t   Lines[LINES][HT1632_COLS][HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS];

// Block-Buffers:
// Controller(16 Byte) * Controllers per Module (4) * Modules in Chain (2) = 128 Byte
uint8_t   Blocks[NUM_BLOCKS][HT1632_COLS][HT1632_BUFSIZE*HT1632_NUM*CHAIN_MODS];



bool EnableLineScroll[LINES] = { false, false };        // Enables automatic Line-Scrolling

// The Add-Buffer
typedef struct {
  bool      IsEmpty;
  uint8_t   Char;
  uint8_t   CharPos;
  uint8_t   Color;
} tyAddBuffer;

tyAddBuffer AddBuffer[LINES];

/************************************************************
 * Initialises Unit
 ************************************************************/
void Buffer_Init() {
  memset(&Blocks,0x00,sizeof(Blocks));
  memset(&Lines,0x00,sizeof(Lines));
  Buffer_AddBuffInit();
}
/************************************************************
 * Initialises the Add-Buffer
 ************************************************************/
void Buffer_AddBuffInit() {
  for (uint8_t i=0;i<LINES;i++) {
    AddBuffer[i].IsEmpty  = true;
    AddBuffer[i].Char     = 0;
    AddBuffer[i].CharPos  = 0;
    AddBuffer[i].Color    = 0;
  }
}
/************************************************************
 * Adds a Char to a lines AddBuffer
 ************************************************************/
bool Buffer_AddChar(uint8_t Line, uint8_t Char, uint8_t Color) {
  if (Line>=LINES) return(false);
  if (AddBuffer[Line].IsEmpty==false) return(false);

  AddBuffer[Line].Char    = Char;
  AddBuffer[Line].Color   = Color;
  AddBuffer[Line].CharPos = 0;
  AddBuffer[Line].IsEmpty = false;
}
/************************************************************
 * Is Add-Buffer ready?
 ************************************************************/
bool Buffer_IsBuffEmpty(uint8_t Line) {
  return(AddBuffer[Line].IsEmpty);
}
/************************************************************
 * Setter/Getter for the LineScrolling
 ************************************************************/
void Buffer_ScrollOn(uint8_t Line)  { EnableLineScroll[Line] = true; }
void Buffer_ScrollOff(uint8_t Line) { EnableLineScroll[Line] = false; }
/************************************************************
 * Create the Buffers and send it to the Display
 ************************************************************/
void Buffer_Refresh() {
  Buffer_Line2HT1632();
  Buffer_SendBlockData();
}
/************************************************************
 * Clear a Line
 ************************************************************/
void Buffer_ClearLine(uint8_t Line) {
  memset(&Lines[Line],0x00,sizeof(Lines[Line]));
}
/************************************************************
 * Worker, called from a ISR
 * Adds buffered Chars or scrolls the Display
 ************************************************************/
void Buffer_ISRWorker() {
  for (uint8_t Line=0;Line<LINES;Line++) {
    if (!AddBuffer[Line].IsEmpty) {
      if (AddBuffer[Line].CharPos<6) {
        uint8_t Byte1 = 0;
        uint8_t Byte2 = 0;
        if ((AddBuffer[Line].Color==TEXTCOL_G)||(AddBuffer[Line].Color==TEXTCOL_O)) Byte1=reverse(font[AddBuffer[Line].Char-' '][AddBuffer[Line].CharPos]);
        if ((AddBuffer[Line].Color==TEXTCOL_R)||(AddBuffer[Line].Color==TEXTCOL_O)) Byte2=reverse(font[AddBuffer[Line].Char-' '][AddBuffer[Line].CharPos]);
        Buffer_AppendByte(Line, Byte1, Byte2);
        AddBuffer[Line].CharPos++;
      } else {
        Buffer_AppendByte(Line, 0x00, 0x00);
        AddBuffer[Line].IsEmpty  = true;
        AddBuffer[Line].Char     = 0;
        AddBuffer[Line].CharPos  = 0;
        AddBuffer[Line].Color    = 0;
      }
    } else {
      if (EnableLineScroll[Line]) Buffer_ScrollLeft(Line,true);
    }
  }
  Buffer_Refresh();
}
/************************************************************
 * Reverse the Bits in a Byte
 ************************************************************/
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
/************************************************************
 * Appends a full Char to the End of the Line
 ************************************************************/
void Buffer_AppendChar(uint8_t Line, uint8_t Char, uint8_t Color) {
  if (Line>=LINES) return;
  for (uint8_t i=0;i<6;i++) {
    uint8_t Byte1 = 0x00;
    uint8_t Byte2 = 0x00;
    if ((Color==TEXTCOL_G)||(Color==TEXTCOL_O)) Byte1=reverse(font[Char-' '][i]);
    if ((Color==TEXTCOL_R)||(Color==TEXTCOL_O)) Byte2=reverse(font[Char-' '][i]);
    Buffer_AppendByte(Line, Byte1, Byte2);
  }
}
/************************************************************
 * Appends a Byte to the end of a Line
 ************************************************************/
void Buffer_AppendByte(uint8_t Line, uint8_t Byte1, uint8_t Byte2 ) {
  if (Line>=LINES) return;
  Buffer_ScrollLeft(Line, false); // Shift Line to the Left
  Lines[Line][0][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1] = Byte1;
  Lines[Line][1][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1] = Byte2;
}
/************************************************************
 * Copys the Line-Buffers into the Block-Buffer
 ************************************************************/
void Buffer_Line2HT1632() {
  /* The Lines must be reordered to match the Controller
   * Order of the Lines:
   * 01 02 03 04 05 06 07 08
   * 09 10 11 12 13 14 15 16
   * 
   * Order of the Controllers:
   * 01 02 05 06 09 10 13 14
   * 03 04 07 08 11 12 15 16
   */
  for (uint8_t Col=0;Col<HT1632_COLS;Col++) {
    memcpy(&Blocks[0][Col][0*HT1632_BUFSIZE], &Lines[0][Col][0*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 01+02 Top Line, 1st Quarter
    memcpy(&Blocks[0][Col][4*HT1632_BUFSIZE], &Lines[0][Col][2*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 05+06 Top Line, 2nd Quarter
    memcpy(&Blocks[1][Col][0*HT1632_BUFSIZE], &Lines[0][Col][4*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 09+10 Top Line, 3nd Quarter
    memcpy(&Blocks[1][Col][4*HT1632_BUFSIZE], &Lines[0][Col][6*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 09+10 Top Line, 3nd Quarter
    memcpy(&Blocks[0][Col][2*HT1632_BUFSIZE], &Lines[1][Col][0*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 03+04 Bottom Line, 1st Quarter
    memcpy(&Blocks[0][Col][6*HT1632_BUFSIZE], &Lines[1][Col][2*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 07+08 Bottom Line, 2nd Quarter
    memcpy(&Blocks[1][Col][2*HT1632_BUFSIZE], &Lines[1][Col][4*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 11+12 Bottom Line, 3nd Quarter
    memcpy(&Blocks[1][Col][6*HT1632_BUFSIZE], &Lines[1][Col][6*HT1632_BUFSIZE], 2*HT1632_BUFSIZE);   // 15+16 Bottom Line, 3nd Quarter
  } 
}
/************************************************************
 * Send the Blockbuffers to the Display
 ************************************************************/
void Buffer_SendBlockData() {
  uint8_t Block = 0;

  for (uint8_t Block=0;Block<NUM_BLOCKS;Block++) {
    for (uint8_t Controller=0;Controller<(CHAIN_MODS*HT1632_NUM);Controller++) {
      HT1632_CS(Controller+1+(Block*CHAIN_MODS*HT1632_NUM));
      HT1632_WAddr(0x00);
      for (uint8_t Col=0;Col<HT1632_COLS;Col++) {           // Colors
        for (uint8_t i=0;i<HT1632_BUFSIZE;i++) {            // Single Controller Memory
          uint16_t BufPos = (Controller*HT1632_BUFSIZE)+i;
          if (BufPos > sizeof(Blocks[Block][Col])) {
            Serial.println("ERROR: Out of Array Access!");
          } else {
            HT1632_WByte(Blocks[Block][Col][BufPos]);
          }
        } // i
      } // Col
      HT1632_CS(0);
    } // Controller
  } // Block
}
/************************************************************
 * Scroll Line Left
 * Loop: Copy char from the left to the right side
 ************************************************************/
void Buffer_ScrollLeft(uint8_t Line, bool Loop) {
  uint8_t   OldValue[2] = { 0, 0 };
  if (Line>=LINES) return;

  if (Loop) { 
    OldValue[0] = Lines[Line][0][0];
    OldValue[1] = Lines[Line][1][0];
  }
  
  for (uint16_t Pos=0;Pos<(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1;Pos++) {
    Lines[Line][0][Pos] = Lines[Line][0][Pos+1];
    Lines[Line][1][Pos] = Lines[Line][1][Pos+1];
  }

  Lines[Line][0][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1] = OldValue[0];
  Lines[Line][1][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1] = OldValue[1];
}

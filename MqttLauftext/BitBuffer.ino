/*
 * Functions for the Linebuffer and Matrixbuffer Handling
 * 
 * 2020 F. Brandner
 */

#include "letters.h"

#define LINES   2         // Number of Lines
#define UPDATERATE      50                // Update Rate for the Matrix

// The Line-Buffers:
// Controller(16 Byte) * Controllers per Line (2) * Modules in Chain (2) * Blocks (2) = 128 Byte (per Color and Line)
uint8_t   Lines[LINES][HT1632_COLS][HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS];

// Block-Buffers:
// Controller(16 Byte) * Controllers per Module (4) * Modules in Chain (2) = 128 Byte (per Color and Block)
uint8_t   Blocks[NUM_BLOCKS][HT1632_COLS][HT1632_BUFSIZE*HT1632_NUM*CHAIN_MODS];

int8_t    LineScroll[LINES] = { 0, 0 };         // Enables automatic Line-Scrolling Left or Right

// The Add-Buffer to add one char after another to the Matrix
// The Applications adds Chars to this Buffer, the ISR will then put them on the Matrix
typedef struct {
  bool      IsEmpty;    // There is a Char in the buffer!
  uint8_t   Char;       // Char to send to the Display
  uint8_t   CharPos;    // Position in the raw Letterdata
  uint8_t   Color;      // Color to use
} tyAddBuffer;
tyAddBuffer AddBuffer[LINES];

/************************************************************
 * Initialises Unit
 ************************************************************/
void BitBuffer_Init() {
  memset(&Blocks,0x00,sizeof(Blocks));
  memset(&Lines,0x00,sizeof(Lines));
  BitBuffer_AddBuffInit();
}
/************************************************************
 * Initialises the Add-Buffer
 ************************************************************/
void BitBuffer_AddBuffInit() {
  for (uint8_t i=0;i<LINES;i++) {
    AddBuffer[i].IsEmpty  = true;
    AddBuffer[i].Char     = 0;
    AddBuffer[i].CharPos  = 0;
    AddBuffer[i].Color    = 0;
  }
}
/************************************************************
 * Adds a Char to a Line, it will be appended on the Right
 * The text will scroll to the left to fit into the matrix
 ************************************************************/
bool BitBuffer_AddChar(uint8_t Line, uint8_t Char, uint8_t Color) {
  if (AddBuffer[Line].IsEmpty==false)   { return(false); } // No debug out for this, we use it often!

  // The following errors will still return true to avoid blocking the calling function
  if (Line>=LINES)                      { Serial.println("Addchar: Illegal Line!"); return(true); }
  if (Char>'~')                         { Serial.print("Illegal Char: 0x"); Serial.println(Char, HEX); return(true); }  
  if (Char<' ')                         { Serial.print("Illegal Char: 0x"); Serial.println(Char, HEX); return(true); }

  AddBuffer[Line].Char    = Char;
  AddBuffer[Line].Color   = Color;
  AddBuffer[Line].CharPos = 0;
  AddBuffer[Line].IsEmpty = false;

  return(true);
}
/************************************************************
 * Is Add-Buffer ready?
 ************************************************************/
bool BitBuffer_IsBuffEmpty(uint8_t Line) {
  return(AddBuffer[Line].IsEmpty);
}
/************************************************************
 * Enable/Disable the automatic LineScrolling
 ************************************************************/
void BitBuffer_SetScroll(uint8_t Line, int8_t Setting) {
  if (Line>=Lines) return;
  LineScroll[Line] = Setting;
}

/************************************************************
 * Create the Buffers and send it to the Display
 ************************************************************/
void BitBuffer_Refresh() {
  BitBuffer_Line2HT1632();
  BitBuffer_SendBlockData();
}
/************************************************************
 * Clear a Linebuffer
 ************************************************************/
void BitBuffer_ClearLine(uint8_t Line) {
  memset(&Lines[Line],0x00,sizeof(Lines[Line]));
}
/************************************************************
 * Worker, called from a ISR
 * Adds buffered Chars or scrolls the Display
 * Behind every Char a empty Column will be added
 ************************************************************/
void BitBuffer_ISRWorker() {
  static unsigned long LastRun = 0;
  if ((millis()-LastRun)<UPDATERATE) return; // Abort if not yet to run

  LastRun=millis();

  for (uint8_t Line=0;Line<LINES;Line++) {                                            // For all Lines
    if (!AddBuffer[Line].IsEmpty) {                                                   // Buffer contains Data?
      if (AddBuffer[Line].CharPos<Letters[AddBuffer[Line].Char-' '].NumOfBytes) {
        uint8_t Byte1 = 0;
        uint8_t Byte2 = 0;
       
        if ((AddBuffer[Line].Color==TEXTCOL_G)||(AddBuffer[Line].Color==TEXTCOL_O)) Byte1=Letters[AddBuffer[Line].Char-' '].Data[AddBuffer[Line].CharPos];
        if ((AddBuffer[Line].Color==TEXTCOL_R)||(AddBuffer[Line].Color==TEXTCOL_O)) Byte2=Letters[AddBuffer[Line].Char-' '].Data[AddBuffer[Line].CharPos];
        BitBuffer_AppendByteR(Line, Byte1, Byte2);
        AddBuffer[Line].CharPos++;
      } else {
        BitBuffer_AppendByteR(Line, 0x00, 0x00);
        AddBuffer[Line].IsEmpty  = true;
        AddBuffer[Line].Char     = 0;
        AddBuffer[Line].CharPos  = 0;
        AddBuffer[Line].Color    = 0;
      }
    } else {
      if (LineScroll[Line]<0) BitBuffer_ScrollLeft(Line,true);
      if (LineScroll[Line]>0) BitBuffer_ScrollRight(Line,true);
    }
  }
  BitBuffer_Refresh();
}
/************************************************************
 * Reverse the Bits in a Byte. Flipping a char upside down
 ************************************************************/
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
/************************************************************
 * Appends a full Char to the end of the Line (direct access)
 * Char will be appended on the right side, text will scroll left
 * Direct Access: No buffering, no matrix update
 ************************************************************/
void BitBuffer_AppendCharR(uint8_t Line, uint8_t Char, uint8_t Color) {
  if (Line>=LINES) return;
  for (uint8_t i=0;i<Letters[Char-' '].NumOfBytes;i++) {
    uint8_t Byte1 = 0x00;
    uint8_t Byte2 = 0x00;
    if ((Color==TEXTCOL_G)||(Color==TEXTCOL_O)) Byte1=Letters[Char-' '].Data[i];
    if ((Color==TEXTCOL_R)||(Color==TEXTCOL_O)) Byte2=Letters[Char-' '].Data[i];
    BitBuffer_AppendByteR(Line, Byte1, Byte2);
  }
}
/************************************************************
 * Appends a Byte to the right end of a Line (direct access)
 * Byte will be appended on the right side, text will scroll left
 * Direct Access: No buffering, no matrix update
 ************************************************************/
void BitBuffer_AppendByteR(uint8_t Line, uint8_t Byte1, uint8_t Byte2 ) {
  if (Line>=LINES) return;
  BitBuffer_ScrollLeft(Line, false); // Shift Line to the Left
  Lines[Line][0][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1] = Byte1;
  Lines[Line][1][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1] = Byte2;
}
/************************************************************
 * Appends a full Char to the start of the Line (direct access)
 * Char will be appended on the left side, text will scroll right
 * Direct Access: No buffering, no matrix update
 ************************************************************/
void BitBuffer_AppendCharL(uint8_t Line, uint8_t Char, uint8_t Color) {
  if (Line>=LINES) return;
  for (int8_t i=Letters[Char-' '].NumOfBytes-1;i>=0;i--) {
    uint8_t Byte1 = 0x00;
    uint8_t Byte2 = 0x00;
    if ((Color==TEXTCOL_G)||(Color==TEXTCOL_O)) Byte1=Letters[Char-' '].Data[i];
    if ((Color==TEXTCOL_R)||(Color==TEXTCOL_O)) Byte2=Letters[Char-' '].Data[i];
    BitBuffer_AppendByteL(Line, Byte1, Byte2);
  }
}
/************************************************************
 * Appends a Byte to the left end of a Line (direct access)
 * Byte will be appended on the left side, text will scroll right
 * Direct Access: No buffering, no matrix update
 ************************************************************/
void BitBuffer_AppendByteL(uint8_t Line, uint8_t Byte1, uint8_t Byte2 ) {
  if (Line>=LINES) return;
  BitBuffer_ScrollRight(Line, false); // Shift Line to the Left
  Lines[Line][0][0] = Byte1;
  Lines[Line][1][0] = Byte2;
}
/************************************************************
 * Copy/Convert the Line-Buffers into the Block-Buffer
 ************************************************************/
void BitBuffer_Line2HT1632() {
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
void BitBuffer_SendBlockData() {
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
 * Depending on "Loop" the left column is either discarded,
 * or appended on the right side
 ************************************************************/
void BitBuffer_ScrollLeft(uint8_t Line, bool Loop) {
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
/************************************************************
 * Scroll Line Right
 * Depending on "Loop" the right column is either discarded,
 * or appended on the left side
 ************************************************************/
void BitBuffer_ScrollRight(uint8_t Line, bool Loop) {
  uint8_t   OldValue[2] = { 0, 0 };
  if (Line>=LINES) return;

  if (Loop) { 
    OldValue[0] = Lines[Line][0][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1];
    OldValue[1] = Lines[Line][1][(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1];
  }
  
  for (uint16_t Pos=(HT1632_BUFSIZE*2*CHAIN_MODS*NUM_BLOCKS)-1;Pos>0;Pos--) {
    Lines[Line][0][Pos] = Lines[Line][0][Pos-1];
    Lines[Line][1][Pos] = Lines[Line][1][Pos-1];
  }

  Lines[Line][0][0] = OldValue[0];
  Lines[Line][1][0] = OldValue[1];
}

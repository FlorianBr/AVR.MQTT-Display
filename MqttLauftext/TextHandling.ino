/*
 * Text/String-Handling
 * 
 * Handles the Stringbuffers to be displayed on the Matrix
 * 
 * 2020 F. Brandner
 */

#define   LINES         2       // Number of Lines
#define   BUFSIZE       50      // Number of elements per Line
#define   ISR_FRAMERATE 100     // Interrupt FrameRate

// The String-Buffer
typedef struct {
  uint8_t   Pos;                // Write Position
  uint8_t   NumOfChars;         // Number of Chars in the Buffer
  uint8_t   ColSelect;          // Currently selected Color
  uint8_t   Char[BUFSIZE];      // Char Data
  uint8_t   Color[BUFSIZE];     // Color Data
} tyTextBuffer;

tyTextBuffer TextBuffer[LINES];

/************************************************************
 * Initialises Unit
 ************************************************************/
void Text_Init() {
  for (uint8_t i=0;i<LINES;i++) Text_ClrLine(i);
}
/************************************************************
 * Clears a Line
 ************************************************************/
void Text_ClrLine(uint8_t Line) {
  if (Line>=LINES) return;
  TextBuffer[Line].Pos        = 0;
  TextBuffer[Line].NumOfChars = 0;
  TextBuffer[Line].ColSelect  = TEXTCOL_G;
}
/************************************************************
 * Sets Text of the Line and parses control-codes
 * Old text will be overwritten
 * 
 * WARNING: SetString will append the chars left. So the
 * chars will be in the opposite order!
 ************************************************************/
void Text_SetString(uint8_t Line, const char* pInputSpring) {
  char * pChar  = NULL;
  char    cBuffer[50];
  uint8_t BufPos=0;
  
  uint8_t Color = TEXTCOL_G;
  if (Line>=LINES) return;
  if (strlen(pInputSpring)<=0) return;

  // Re-Arrange String. Reverse the chars
  memset(&cBuffer[0],0,50);
  pChar = pInputSpring;
  BufPos = strlen(pInputSpring)-1;
  for (uint8_t i=0;i<strlen(pInputSpring);i++) {
    cBuffer[BufPos] = *pChar;
    pChar++;
    BufPos--;
  }

  BitBuffer_ClearLine(Line);
  pChar = &cBuffer[0];
  for (uint8_t i=0;i<strlen(pInputSpring);i++) {

    if (*pChar=='\\') {                   // Control command?
      pChar++;                            // next char...
      i++;

      // The color commands
      if ((*pChar=='g')||(*pChar=='G')) Color = TEXTCOL_G;
      if ((*pChar=='r')||(*pChar=='R')) Color = TEXTCOL_R;
      if ((*pChar=='o')||(*pChar=='O')) Color = TEXTCOL_O;

      // an escaped backslash
      if (*pChar=='\\') BitBuffer_AppendCharL(Line, *pChar, Color);

    } else {
      BitBuffer_AppendCharL(Line, *pChar, Color);
      BitBuffer_AppendByteL(Line, 0x00, 0x00); // Empty column behind every char
    }
    pChar++;
  }
}
/************************************************************
 * Adds Text to the Line and parses control-codes
 ************************************************************/
void Text_AddString(uint8_t Line, const char* pInputSpring) {
  char * pChar  = NULL;
  if (Line>=LINES) return;
  if (strlen(pInputSpring)<=0) return;

  pChar = pInputSpring;
  for (uint8_t i=0;i<strlen(pInputSpring);i++) {

    if (*pChar=='\\') {                   // Control command?
      pChar++;                            // next char...
      i++;

      // The color commands
      if ((*pChar=='g')||(*pChar=='G')) TextBuffer[Line].ColSelect  = TEXTCOL_G;
      if ((*pChar=='r')||(*pChar=='R')) TextBuffer[Line].ColSelect  = TEXTCOL_R;
      if ((*pChar=='o')||(*pChar=='O')) TextBuffer[Line].ColSelect  = TEXTCOL_O;

      // an escaped backslash
      if (*pChar=='\\') Text_AddChar(Line, *pChar, TextBuffer[Line].ColSelect);

    } else {
      Text_AddChar(Line, *pChar, TextBuffer[Line].ColSelect);
    }
    pChar++;
  }
}
/************************************************************
 * Adds Char and Color to the Buffer
 ************************************************************/
void Text_AddChar(uint8_t Line, const char InputChar, uint8_t InputCol) {
  if (Line>=LINES) return;

  if (TextBuffer[Line].NumOfChars>=BUFSIZE) Text_RemoveChar(Line); // If full, remove one Char

  TextBuffer[Line].NumOfChars++;
  TextBuffer[Line].Char[TextBuffer[Line].Pos] = InputChar;
  TextBuffer[Line].Color[TextBuffer[Line].Pos] = InputCol;

  if (TextBuffer[Line].Pos<(BUFSIZE-1)) TextBuffer[Line].Pos++;
}
/************************************************************
 * Remove the oldest Char from the Buffer
 ************************************************************/
void Text_RemoveChar(uint8_t Line) {
  if (Line>=LINES) return;
  if (TextBuffer[Line].NumOfChars==0) return;

  // Shift Buffer 1 char to the left and clear rightmost char
  for (uint8_t i=0;i<BUFSIZE;i++) {
    TextBuffer[Line].Char[i] = TextBuffer[Line].Char[i+1];
    TextBuffer[Line].Color[i] = TextBuffer[Line].Color[i+1];
  }
  // Clear the freed Position
  TextBuffer[Line].Char[BUFSIZE-1] = 0x00;
  TextBuffer[Line].Color[BUFSIZE-1] = 0x00;

  // Decrease Counters
  if (TextBuffer[Line].NumOfChars>0) TextBuffer[Line].NumOfChars--;
  if (TextBuffer[Line].Pos>0) TextBuffer[Line].Pos--;
}
/************************************************************
 * Debug: Print Buffer to Console
 ************************************************************/
void Text_DebugInfo(uint8_t Line) {
  if (Line>=LINES) return;
  Serial.println("+---------------------");
  Serial.print("| Line ");
  Serial.println(Line,DEC);
  Serial.print("| Position = ");
  Serial.println(TextBuffer[Line].Pos, DEC);
  Serial.print("| NumOfChars = ");
  Serial.println(TextBuffer[Line].NumOfChars, DEC);
  Serial.print("| SelColor = ");
  Serial.println(TextBuffer[Line].ColSelect, DEC);
  Serial.print("| CData  = [");
  for (uint8_t i=0;i<BUFSIZE;i++) { Serial.print(" 0x"); Serial.print(TextBuffer[Line].Char[i],HEX); }
  Serial.println("]");
  Serial.print("| Colors = [");
  for (uint8_t i=0;i<BUFSIZE;i++) { Serial.print(" 0x"); Serial.print(TextBuffer[Line].Color[i],HEX); }
  Serial.println("]");
  Serial.println("----------------------");
}
/************************************************************
 * Worker, called from a ISR
 ************************************************************/
void Text_ISRWorker() {
  static unsigned long LastRun = 0;
  if ((millis()-LastRun)<ISR_FRAMERATE) return; // Abort if not yet to run
  LastRun=millis();

  for (uint8_t i=0;i<LINES;i++) {
    if (TextBuffer[i].NumOfChars>0) {         // There are Chars in the Textbuffer
      if (BitBuffer_AddChar(i,TextBuffer[i].Char[0], TextBuffer[i].Color[0])==true) { // Char added successfully?
        Text_RemoveChar(i);                   // Remove it from the buffer
      }
    }
  }
  return;  
}

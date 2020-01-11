


#define   BUFSIZE       20      // Number of elements

// The String-Buffer
typedef struct {
  bool      IsEmpty;
  bool      IsFull;
  uint8_t   Pos;
  uint8_t   Char[BUFSIZE];
  uint8_t   Color[BUFSIZE];
} tyTextBuffer;

tyTextBuffer TextBuffer[LINES];


/************************************************************
 * Initialises Unit
 ************************************************************/
void Text_Init() {
  for (uint8_t i=0;i<LINES;i++) {
    TextBuffer[i].IsEmpty  = true;
    TextBuffer[i].IsFull   = false;
    TextBuffer[i].Pos      = 0;
  }
}

/************************************************************
 * Worker, called from a ISR
 ************************************************************/
void Text_ISRWorker() {
  return;  
}

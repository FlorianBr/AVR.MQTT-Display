/*
 * MQTT-Controlled Text for Sure Electronics LED Matrix
 * 
 * - Arduino Mega
 * - KBV LCD-Shield
 * - CSA:   53
 *   CSB:   45
 *   CLK:   51
 *   WR:    47
 *   DATA:  49
 *   
 * Connected are 4 Matrix Modules, grouped by 2
 * Left is Block A with CSA, Right is Block B with CSB
 * CLK, WR and Data is shared
 *   
 */

#include "ht1632.h"

// Hardware Configuration (Pinning)
#define CSA             53                // Chip Select A-Block  (Low act)
#define CSB             45                // Chip Select B-Block  (Low act)
#define CLK             51                // Chip Select Clock    (High act)
#define DATA            49                // Data Input
#define WR              47                // Write Data Clock

#define TEXTCOL_G       1                 // Color = Green
#define TEXTCOL_R       2                 // Color = Red
#define TEXTCOL_O       3                 // Color = Green&Red=Orange

void setup() {
  // Some debug info
  Serial.begin(115200);
  Serial.println("Setting up...");
  Serial.println("Compiled: " __DATE__ " " __TIME__);

  // Setup Pins to Matrix
  pinMode(CSA,  OUTPUT);  digitalWrite(CSA, HIGH);
  pinMode(CSB,  OUTPUT);  digitalWrite(CSB, HIGH);
  pinMode(CLK,  OUTPUT);  digitalWrite(CLK, HIGH);
  pinMode(DATA, OUTPUT);  digitalWrite(DATA,HIGH);
  pinMode(WR,   OUTPUT);  digitalWrite(WR,  HIGH);

  // Setup Matrix
  HT1632_WCmd(-1, SYS_EN);
  HT1632_WCmd(-1, LED_OFF);
  HT1632_WCmd(-1, BLINK_OFF);
  HT1632_SetPWMVal(0);
  HT1632_Clear();

  // Setup Timer interrupt
  cli();
  TCCR1A = 0;                           // Reset Registers
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 31249;                        // compare match register
  TCCR1B |= (1 << CS10);                // Prescaler: /1
  TCCR1B |= (1 << WGM12);               // CTC mode
  TIMSK1 |= (1 << OCIE1A);              // enable timer compare interrupt
  sei();

  Serial.println("Setting up complete!");
}

void loop() {
  static char InputText[] = { "Hello World!" };
  char * pTextChar = &InputText[0];
  
  uint8_t iChar = 0;
  uint8_t iStringPos = 0;
  Buffer_Init();
  Text_Init();
  HT1632_WCmd(-1,LED_ON);

  while(1) {
    if (Buffer_IsBuffEmpty(0)) {
      Buffer_AddChar(0, ' '+iChar, 1+iChar%3);
      iChar++;
    }
    if (iChar>=96) iChar=0;

    if (Buffer_IsBuffEmpty(1)) {
      if (pTextChar!=NULL) {
        Buffer_AddChar(1, *pTextChar, TEXTCOL_G);
        pTextChar++;
        if (pTextChar>=(&InputText[0]+sizeof(InputText))-1) { pTextChar=NULL; Buffer_ScrollOn(1); }
      }
    }

    delay(5);
  }
}

/**************************************** ISR */
ISR(TIMER1_COMPA_vect) {
  static unsigned long LastRun = 0;

  if ((millis()-LastRun)>=50) {
    LastRun=millis();
    Text_ISRWorker();
    Buffer_ISRWorker(); 
  }
}

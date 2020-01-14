/*
 * MQTT-Controlled Text for Sure Electronics LED Matrix
 * 
 * 2020 F. Brandner
 * 
 * HW Setup::
 *      Arduino Mega
 *      KBV LCD-Shield (currently unused)
 *      4x Sure Electronics LED matrix displays, Two chained in 2 blocks
 *            Block A is on the left with CSA, Block B is right with CSB
 *      
 *  
 * Pinning:
 *   CSA:   53      Chip-Select for Block A
 *   CSB:   45      Chip-Select for Block B
 *   CLK:   51      Clock (Shared)
 *   WR:    47      Write (Shared)
 *   DATA:  49      Data (Shared)
 *   
 *   
 * TODO:  
 *    - Improve HT1632-Functions (Timing)
 *    - Handling of Text/Strings
 *    - MQTT Connection  (Library: PubSub)
 *    - Port everything to ESP32
 */

#include "ht1632.h"

// Hardware Configuration (Pinning)
#define CSA             53                // Chip Select A-Block  (Low act)
#define CSB             45                // Chip Select B-Block  (Low act)
#define CLK             51                // Chip Select Clock    (High act)
#define DATA            49                // Data Input
#define WR              47                // Write Data Clock

/**************************************** Setup */
void setup() {
  // Some debug info
  Serial.begin(57600);
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

  BitBuffer_Init();                     // Init Modules
  Text_Init();

  Serial.println("Setting up complete!");
}
/**************************************** Loop */
void loop() {
  uint8_t   LineSelect = 0;
  uint32_t  Uptime=0;
  

  HT1632_WCmd(-1,LED_ON);               // Enable Display Output

  Text_SetString(1, "Hello World!");
  BitBuffer_SetScroll(1, 1); // Scroll Right
  
  while(1) {
#if 1
      // TESTMODE!!
      char  cBuffer[50];
      uint16_t    Secs;
      uint16_t    MSecs;

      MSecs = millis();
      Secs  = MSecs/1000;
      MSecs = MSecs - 1000*Secs;

      snprintf(&cBuffer[0], 50, "T=o\\%d.%03dg\\", Secs, MSecs);
      Text_SetString(0, &cBuffer[0]);


#endif


#if 0  
    if (Serial.available() > 0) {
      char  Buffer[100];
      Serial.setTimeout(5000);
      String InputStr = Serial.readStringUntil('\n');

      Serial.print("Line ");
      Serial.print(LineSelect);
      Serial.print(" is selected, received: [");
      Serial.print(InputStr);
      Serial.println("]");

      InputStr.toCharArray(Buffer,50);

      if (InputStr.equals("L1")) {            // Command: Select Line 1
        Serial.println("Selected Line = 1");  
        LineSelect=0;
      } else 
      if (InputStr.equals("L2")) {            // Command: Select Line 2
        Serial.println("Selected Line = 2");  
        LineSelect=1;
      } else 
      if (InputStr.equals("C1")) {            // Command: Clear Line 1
        Text_ClrLine(LineSelect);
        BitBuffer_ClearLine(LineSelect);
      } else 
      if (InputStr.equals("C2")) {            // Command: Clear Line 2
        Text_ClrLine(LineSelect);
        BitBuffer_ClearLine(LineSelect);
      } else 
      if (InputStr.startsWith("A")) {         // Command: Append Text
        Serial.println("Append Text");
        Text_AddString(LineSelect, &Buffer[1]);
      } else 
      if (InputStr.startsWith("S")) {         // Command: Set Text
        Serial.println("Set Text");  
        Text_SetString(LineSelect, &Buffer[1]);
      } else 
      if (InputStr.startsWith("I")) {         // Command: Info
        Text_DebugInfo(LineSelect);
      } else  {                               // Everything else is send to the Textparser
        Serial.println("ERROR: Unknown Command");  
      }
    }
#endif
    delay(100);
  }
}
/**************************************** ISR */
ISR(TIMER1_COMPA_vect) {
  Text_ISRWorker();
  BitBuffer_ISRWorker(); 
}

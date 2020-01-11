/*
 * Functions to access Sure Electronics LED Matrix Displays with HT1632 Controller
 */

#define CLKDLY          void();                 // Delay between digitalWrites (if necessary)

/************************************************************
 * Generate a Clock-Pulse
 ************************************************************/
void HT1632_CLK_Pulse() {
  digitalWrite(CLK, HIGH);
  CLKDLY;
  digitalWrite(CLK, LOW);
  CLKDLY;
}
/************************************************************
 * Clears the Matrix, no matter whats in the Buffer
 ************************************************************/
void HT1632_Clear() {
  HT1632_CS(-1);
  HT1632_WAddr(0x00);
  for (uint8_t Adr=0; Adr<HT1632_BUFSIZE;Adr++) HT1632_WByte(0x00);
  HT1632_CS(0);
}
/************************************************************
 * Sets the LED PWM Value for all Modules
 ************************************************************/
void HT1632_SetPWMVal(uint8_t PWMValue) {
  uint16_t Value;
  Value  = PWM_DUTY;
  Value |= (PWMValue&0x0000F);
  HT1632_WCmd(-1, Value);
}
/************************************************************
 * Selects a Module in a Block
 *  0 = NONE
 * -1 = ALL
 * >0 = Module Nr.
 ************************************************************/
void HT1632_CS(int8_t Module) {
  if (Module<0) {               // Enable all Modules in both Blocks
    digitalWrite(CSA, LOW);
    digitalWrite(CSB, LOW);
    CLKDLY;
    for (uint8_t i=0;i<(CHAIN_MODS*HT1632_NUM);i++) HT1632_CLK_Pulse();
  } else
  if (Module==0) {              // Disable all Modules in both Blocks
    digitalWrite(CSA, HIGH);
    digitalWrite(CSB, HIGH);
    CLKDLY;
    for (uint8_t i=0;i<(CHAIN_MODS*HT1632_NUM);i++) HT1632_CLK_Pulse();
  } else {                      // Select Module
    uint8_t i;
    uint8_t BlockModule;

    BlockModule=Module;
    if (BlockModule>(HT1632_NUM*CHAIN_MODS)) BlockModule-=(HT1632_NUM*CHAIN_MODS); // Number for Block B

    // Disable all modules first
    digitalWrite(CSA, HIGH);
    digitalWrite(CSB, HIGH);
    CLKDLY;
    for (i=0;i<(CHAIN_MODS*HT1632_NUM);i++) HT1632_CLK_Pulse();
  
    if (Module>(HT1632_NUM*CHAIN_MODS)) { // Block B
      digitalWrite(CSB, LOW);
      CLKDLY;
      HT1632_CLK_Pulse();
      digitalWrite(CSB, HIGH);
      CLKDLY;
    } else {                              // Block A
      digitalWrite(CSA, LOW);
      CLKDLY;
      HT1632_CLK_Pulse();
      digitalWrite(CSA, HIGH);
      CLKDLY;
    }
    i=1;
    for(;i<BlockModule;i++) HT1632_CLK_Pulse();
  }
}
/************************************************************
 * Send a Command (12 Bits) to a Module
 ************************************************************/
void  HT1632_WCmd(int8_t Module, uint16_t Cmd) {
  uint16_t CmdMasked = Cmd & 0x0FFF; // Only 12 Bit

  HT1632_CS(0);  // Disable ALL
  CLKDLY;
  HT1632_CS(Module); // Enable selected
  CLKDLY;
  
  for (uint8_t i=0;i<12;i++) {
    uint16_t Bit;
    
    digitalWrite(WR, LOW);
    CLKDLY;
    Bit = CmdMasked & 0x0800;
    CmdMasked = CmdMasked << 1;
    Bit = Bit >> 11;

    digitalWrite(DATA, Bit==1);
    CLKDLY;
  
    digitalWrite(WR, HIGH);
    CLKDLY;
  }
  HT1632_CS(0);  // Disable ALL
}
/************************************************************
 * Writes Address (7 Bits) to a previously selected Module
 ************************************************************/
void  HT1632_WAddr(uint8_t Address) {
  digitalWrite(WR, LOW);
  CLKDLY;
  digitalWrite(DATA, HIGH);     // "1"
  CLKDLY;
  digitalWrite(WR, HIGH);
  CLKDLY;

  digitalWrite(WR, LOW);
  CLKDLY;
  digitalWrite(DATA, LOW);      // "0"
  CLKDLY;
  digitalWrite(WR, HIGH);
  CLKDLY;

  digitalWrite(WR, LOW);
  CLKDLY;
  digitalWrite(DATA, HIGH);     // "1"
  CLKDLY;
  digitalWrite(WR, HIGH);
  CLKDLY;

  for (uint8_t i=0;i<7;i++) {
    uint16_t Bit;
    digitalWrite(WR, LOW);
    CLKDLY;

    Bit = Address & 0x40;
    Address = Address << 1;
    Bit = Bit >> 6;

    digitalWrite(DATA, Bit==1);
    CLKDLY;
    digitalWrite(WR, HIGH);
    CLKDLY;
  }
}
/************************************************************
 * Send Data (1 Byte) to a previously selected Module
 ************************************************************/
void  HT1632_WByte(uint8_t Byte) {
  HT1632_WNibble(UPPER(Byte));
  HT1632_WNibble(LOWER(Byte));
}
/************************************************************
 * Send Data (4 Bits) to a previously selected Module
 ************************************************************/
void  HT1632_WNibble(uint8_t Data) {
  uint8_t Temp = Data&0x0F;

  for (uint8_t i=0;i<4;i++) {
    uint16_t Bit;
    
    digitalWrite(WR, LOW);
    CLKDLY;
    Bit = Temp & 0x08;
    Temp = Temp << 1;
    Bit = Bit >> 3;

    digitalWrite(DATA, Bit==1);
    CLKDLY;
    digitalWrite(WR, HIGH);
    CLKDLY;
  }
}

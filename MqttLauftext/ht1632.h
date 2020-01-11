/*
 * Defines to be used with the HT1632 Controller of the LED Matrix Displays
 * 
 * 2020 F. Brandner
 */
 
// Command Codes
#define SYS_DIS         0b100000000000              // Turn off osc (disable everything)
#define SYS_EN          0b100000000010              // Turn on osc
#define LED_OFF         0b100000000100              // Turn OFF LED duty cycle gen
#define LED_ON          0b100000000110              // Turn ON LED duty cycle gen
#define BLINK_OFF       0b100000010000              // Blinking OFF
#define BLINK_ON        0b100000010010              // Blinking ON
#define PWM_DUTY        0b100101000000              // PWM Duty Cycle

// Hardware-Settings
#define HT1632_BUFSIZE  2*8                         // Memorysize of one Controller
#define HT1632_NUM      4                           // Number of Controllers per Module
#define HT1632_COLS     2                           // Number of Colors
#define CHAIN_MODS      2                           // Number of chained Modules
#define NUM_BLOCKS      2                           // Number of Blocks

// Helpers
#define LOWER(x)        ((byte)(x&0x0F))            // Return lower nibble
#define UPPER(x)        ((byte)((x&0xF0)>>4))       // Return higher nibble

/*
 * All the letters/chars for the matrix
 * Currenty unused!
 */

typedef struct {
  uint8_t NumOfBytes;
  uint8_t Data[8];
} tyLetter;

static tyLetter Letters[] = {
    { 5, 0x00, 0x00, 0x00, 0x00, 0x00 },        // 0x20 Space
    { 2, 0x00, 0xFD },                          //      !
    { 4, 0x00, 0xC0, 0x00, 0xC0 },              //      "
    { 6, 0x00, 0x28, 0xFE, 0x28, 0xFE, 0x28 },  //      #
    { 5, 0x00, 0x74, 0xF2, 0x9E, 0x4C },        //      $
    { 5, 0x00, 0xC6, 0x18, 0x20, 0xC6 },        //      %
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      &       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      '       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      (       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      )       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        // 0x2A *       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      +       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      ,       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      -       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      .       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        // 0x2F /       TODO
    { 5, 0x00, 0x7C, 0x82, 0x82, 0x7C },        // 0x30 0
    { 4, 0x00, 0x42, 0xFE, 0x02 },              //      1
    { 5, 0x00, 0x46, 0x8A, 0x92, 0x62 },        //      2
    { 5, 0x00, 0x44, 0x82, 0x92, 0x6C },        //      3
    { 4, 0x00, 0xF0, 0x10, 0xFE },              //      4
    { 5, 0x00, 0xF4, 0x92, 0x92, 0x8C },        //      5
    { 5, 0x00, 0x7C, 0x92, 0x92, 0x4C },        //      6
    { 5, 0x00, 0x80, 0x80, 0x9E, 0xE0 },        //      7
    { 5, 0x00, 0x6C, 0x92, 0x92, 0x6C },        //      8
    { 5, 0x00, 0x64, 0x92, 0x92, 0x7C },        //      9
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        // 0x3A :       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      ;       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      <       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      =       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      >       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        // 0x3F ?       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        // 0x40 @       TODO
    { 6, 0x00, 0x7E, 0x90, 0x90, 0x90, 0x7E },  //      A
    { 5, 0x00, 0xFE, 0x92, 0x92, 0x6C },        //      B
    { 5, 0x00, 0x7C, 0x82, 0x82, 0x82 },        //      C
    { 5, 0x00, 0xFE, 0x82, 0x82, 0x7C },        //      D
    { 5, 0x00, 0xFE, 0x92, 0x92, 0x92 },        //      E
    { 5, 0x00, 0xFE, 0x90, 0x90, 0x80 },        //      F
    { 5, 0x00, 0x7C, 0x82, 0x92, 0x5C },        //      G
    { 5, 0x00, 0xFE, 0x10, 0x10, 0xFE },        //      H
    { 4, 0x00, 0x82, 0xFE, 0x82 },              //      I
    { 4, 0x00, 0x04, 0x02, 0xFC },              // 0x4A J
    { 5, 0x00, 0xFE, 0x10, 0x28, 0xC6 },        //      K
    { 5, 0x00, 0xFE, 0x02, 0x02, 0x02 },        //      L
    { 6, 0x00, 0xFE, 0x60, 0x10, 0x60, 0xFE },  //      M
    { 5, 0x00, 0xFE, 0x60, 0x18, 0xFE },        //      N
    { 5, 0x00, 0xFE, 0x82, 0x82, 0xFE },        //      O
    { 5, 0x00, 0xFE, 0x90, 0x90, 0x60 },        // 0x50 P
    { 5, 0x00, 0x7C, 0x82, 0x86, 0x7F },        //      Q
    { 5, 0x00, 0xFE, 0x90, 0x90, 0x6E },        //      R
    { 5, 0x00, 0x64, 0x92, 0x92, 0x4C },        //      S
    { 6, 0x00, 0x80, 0x80, 0xFE, 0x80, 0x80 },  //      T
    { 5, 0x00, 0xFC, 0x02, 0x02, 0xFC },        //      U
    { 6, 0x00, 0xE0, 0x18, 0x06, 0x18, 0xE0 },  //      V
    { 6, 0x00, 0xF8, 0x06, 0x38, 0x06, 0xF8 },  //      W
    { 4, 0x00, 0xEE, 0x10, 0xEE },              //      X
    { 4, 0x00, 0xE0, 0x1E, 0xE0 },              //      Y
    { 5, 0x00, 0x86, 0x9A, 0xA2, 0xC2 },        // 0x5A Z
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      [       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      \       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      ]       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        //      ^       TODO
    { 5, 0x00, 0xFE, 0xA2, 0x8A, 0xFE },        // 0x5F _       TODO
};
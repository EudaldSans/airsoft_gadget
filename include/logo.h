// Images can be converted to XBM format by using the online converter here:
// https://www.online-utility.org/image/convert/to/XBM

// The output must be pasted in a header file, renamed and adjusted to appear
// as as a const unsigned char array in PROGMEM (FLASH program memory).

// The xbm format adds padding to pixel rows so they are a whole number of bytes
// In this example 50 pixel width means 56 bits = 7 bytes
// the 50 height then means array uses 50 x 7 = 350 bytes of FLASH
// The library ignores the padding bits when drawing the image on the display.

// Example of the correct format is shown below

#include <pgmspace.h>

// Espressif logo 50 x 50 pixel array in XBM format
#define logoWidth  14  // logo width
#define logoHeight 14  // logo height

// Image is stored in this array
PROGMEM const unsigned char logo[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x07, 0x01, 0x08, 0x6D, 0x1B, 
  0x6D, 0x1B, 0x6D, 0x1B, 0x6D, 0x1B, 0x01, 0x08, 0xFE, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, };
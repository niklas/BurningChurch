#include "Fire.h"

void Fire__init(byte * f, byte l) {
  byte h;
  // make everything cold
  for(h = 0; h < l; h++) {
    f[h] = 0;
  }
}

// @param cooling: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100

// @param sparking: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.

// @param base: How many pixels release new heat sources.

void Fire__eachStep(byte * f, byte l, byte cooling, byte sparking, byte base) {
  byte h;

  // Cool down every cell a little
  for(h = 0; h < l; h++) {
    f[h] = qsub8( f[h], random8(0, ((cooling * 10) / l) + 2));
  }

  //  Heat from each cell drifts 'up' and diffuses a little
  for(h = l - 1; h >= 2; h--) {
    f[h] = (f[h] + f[h - 1] + f[h - 2] ) / 3;
  }

  //  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < sparking ) {
    byte y = random8(base);
    f[y] = qadd8( f[y], random8(160,255) );
  }
}

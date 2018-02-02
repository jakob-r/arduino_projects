#ifndef mytypes_h
#define mytypes_h

typedef struct {
  CRGB color;
  uint8_t x; // position on the ring 0..254
  uint8_t l; // length on 0..254 scale
  int8_t v; // Velocity
} particle;

#endif



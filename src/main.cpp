#include <Arduino.h>
#include <FastLED.h>
#include <Fire.h>

#define LED_TYPE LPD8806
#define STRIP_COLOR_ORDER BRG
#define STRIP_PIXEL_COUNT 12
#define PIN_STRIP_DATA 10
#define PIN_STRIP_CLK 9
#define PIN_DIRT 0

#define FPS 60
#define MIN 2
#define MAX 253
#define CHILL 42
#define DRY_MIN 900
#define DRY_STEP 5

#define DEBUG

CRGB color;
CRGB strip[STRIP_PIXEL_COUNT];

// Fire settings
byte heat[STRIP_PIXEL_COUNT];
byte cooling, sparking, base;

// Holy settings
// ...

int step;

byte chan;
uint8_t dryness;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  color = CRGB::Black;

  FastLED.addLeds<LED_TYPE,PIN_STRIP_DATA,PIN_STRIP_CLK,STRIP_COLOR_ORDER>(strip, STRIP_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.setMaxRefreshRate(FPS);

  Fire__init(heat, STRIP_PIXEL_COUNT);
  cooling = 25;
  sparking = 85;
  base = 4;

  step = 0;

  pinMode(PIN_DIRT, INPUT);

  // Start in the middle
  dryness = 0x7F;
}

void animationStep() {
  int i;
  Fire__eachStep(heat, STRIP_PIXEL_COUNT, cooling, sparking, base);

  for (i; i<STRIP_PIXEL_COUNT; i++) {
    strip[i] = blend(
        HeatColor(heat[i]),
        CRGB(118,195,223),
        dryness
    );
  }
}

void animationNext() {
  delay(1000/FPS);
}

void readDirt() {
  int val = analogRead(PIN_DIRT);
  if (val > DRY_MIN) {
    dryness = qadd8(dryness, DRY_STEP);
  } else {
    dryness = qsub8(dryness, DRY_STEP);
  }
#ifdef DEBUG
  Serial.print("Raw Dryness: ");
  Serial.print( val / 10.23);
  Serial.print("% Dryness: ");
  Serial.print( dryness / 2.55);
  Serial.print("% (");
  Serial.print(dryness);
  Serial.print(")");
  Serial.println();
#endif
}

void loop() {
  FastLED.show();
  animationNext();
  animationStep();

  EVERY_N_MILLIS(222) {
    readDirt();
  }
}

#include <Arduino.h>
#include <FastLED.h>
#include <Fire.h>

#define LED_TYPE LPD8806
#define STRIP_COLOR_ORDER BRG
#define STRIP_PIXEL_COUNT 12

// Fire Interpolation (MUST ADAPT BLEND ALGORYTHM WHEN CHANGING)
#define HEAT_RESOLUTION_FACTOR 4
#define HEAT_RESOLUTION STRIP_PIXEL_COUNT * HEAT_RESOLUTION_FACTOR
#define HEAT_FOCUS 200
#define PIN_STRIP_DATA 10
#define PIN_STRIP_CLK 9
#define PIN_DIRT 0
// Max values
#define FIRE_SPARKING 85

#define FPS 60
#define DRY_MIN 900
// How fast to react to new values 1..64
#define DRY_ADAPTIVNESS 7

// How slow do the holy colors fade
#define HOLY_LAZYNESS 42

#define DEBUG

CRGB strip[STRIP_PIXEL_COUNT];

CRGB blue = CRGB(118,195,223);
CRGB green = CRGB(33,187,26);
CRGB purple = CRGB(201,66,207);
CRGB black  = CRGB::Black;
CRGB white  = CRGB::White;

CRGBPalette16 holyPalette = CRGBPalette16(
    green  , green , green  , blue   ,
    blue   , blue  , purple , purple ,
    purple , green , blue   , purple ,
    blue   , green , purple , white
);

// Fire settings
byte heat[HEAT_RESOLUTION];
byte cooling, sparking, base;

// Holy settings
// ...

uint16_t step;

byte chan;
uint8_t dryness;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  FastLED.addLeds<LED_TYPE,PIN_STRIP_DATA,PIN_STRIP_CLK,STRIP_COLOR_ORDER>(strip, STRIP_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.setMaxRefreshRate(FPS);

  Fire__init(heat, HEAT_RESOLUTION);
  cooling = 25;
  sparking = FIRE_SPARKING;
  base = 4;

  step = 0;

  pinMode(PIN_DIRT, INPUT);

  // Start in the middle
  dryness = 0x7F;
}

void animationStep() {
  int i;
  CRGB fire;
  uint16_t h;
  Fire__eachStep(heat, HEAT_RESOLUTION,
      cooling,
      FIRE_SPARKING - (FIRE_SPARKING * dryness / 255),
      base
  );

  for (i; i<STRIP_PIXEL_COUNT; i++) {
    h = i * HEAT_RESOLUTION_FACTOR;
    fire = blend(
        blend(
          HeatColor(heat[h  ]),
          HeatColor(heat[h+1]),
          HEAT_FOCUS
        ),
        blend(
          HeatColor(heat[h+2]),
          HeatColor(heat[h+3]),
          qsub8(255, HEAT_FOCUS)
        ),
        128
    );
    strip[i] = blend(
        fire,
        ColorFromPalette(holyPalette, step / HOLY_LAZYNESS, 255, LINEARBLEND),
        dryness
    );
  }
}

void animationNext() {
  step++;
  delay(1000/FPS);
}

void readDirt() {
  int val = analogRead(PIN_DIRT);
  if (val > DRY_MIN) {
    dryness = qadd8(dryness, DRY_ADAPTIVNESS);
  } else {
    dryness = qsub8(dryness, DRY_ADAPTIVNESS);
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

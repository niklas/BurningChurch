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
// Max value
#define FIRE_SPARKING 85
#define FIRE_COOLING 55
#define FIRE_BASE 4

#define FPS 60
#define DRY_MIN 900
// How fast to react to new values 1..64
#define DRY_ADAPTIVNESS 7

// How slow do the holy colors fade
#define HOLY_LAZYNESS 23

#define THUNDER_SPEED 130
#define THUNDER_DARKNESS 190
#define THUNDER_DURATION FPS * 5

#define DEBUG

CRGB strip[STRIP_PIXEL_COUNT];

CRGB blue = CRGB(118,195,223);
CRGB green = CRGB(33,187,26);
CRGB purple = CRGB(201,66,207);
CRGB black  = CRGB::Black;
CRGB white  = CRGB::White;

CRGBPalette16 holyPalette = CRGBPalette16(
    green,
    blue,
    purple,
    green,
    green,
    green,
    blue,
    blue,
    blue,
    purple,
    purple,
    purple,
    blue,
    purple,
    white,
    white
    );

// Fire settings
byte heat[HEAT_RESOLUTION];

// Holy settings
// ...

uint16_t step;

byte chan;
uint8_t dryness;

void animationStep() {
  int i;
  CRGB fire;
  uint16_t h;

  // Fire burns (more) when it's wet
  Fire__eachStep(heat, HEAT_RESOLUTION,
      FIRE_COOLING,
      FIRE_SPARKING - (FIRE_SPARKING * dryness / 255),
      FIRE_BASE
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
        ColorFromPalette(holyPalette,
          (8 * (i % 4)) + (step / HOLY_LAZYNESS),
          255, LINEARBLEND),
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

void intro() {
  uint16_t i, p;
  uint16_t val, lim;
  byte on;
  CRGB col;

#ifdef DEBUG
  Serial.println("Running Intro...");
#endif

  for (i=0; i<THUNDER_DURATION; i++) {
    val = inoise8(THUNDER_SPEED * i * i);
    lim = i;
    on = val < lim ? 255 : 0;
    col = CRGB(on, on, on);
    for (p=0; p<STRIP_PIXEL_COUNT; p++) {
      strip[p] = col;
    }
    FastLED.show();
#ifdef DEBUG
    Serial.print(i);
    Serial.print(" -- ");
    Serial.print(val);
    Serial.print("/");
    Serial.print(lim);
    Serial.println();
#endif
    delay(1000/FPS);
  }

  Serial.println("Intro done.");
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  FastLED.addLeds<LED_TYPE,PIN_STRIP_DATA,PIN_STRIP_CLK,STRIP_COLOR_ORDER>(strip, STRIP_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.setMaxRefreshRate(FPS);

  Fire__init(heat, HEAT_RESOLUTION);

  step = 0;

  pinMode(PIN_DIRT, INPUT);

  // Start in the middle
  dryness = 0x7F;

  intro();
}

void loop() {
  FastLED.show();
  animationNext();
  animationStep();

  EVERY_N_MILLIS(222) {
    readDirt();
  }
}

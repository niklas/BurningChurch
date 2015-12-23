#include <Arduino.h>
#include <FastLED.h>
#include <Fire.h>

#define LED_R 6
#define LED_G 5
#define LED_B 3

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

#define DEBUG

CRGB color;
CRGB strip[STRIP_PIXEL_COUNT];

// Fire settings
byte heat[STRIP_PIXEL_COUNT];
byte cooling, sparking, base;

int step;

byte chan;
int dir;
int chill;

void setLED(CRGB color) {
  analogWrite(LED_R, color.red);
  analogWrite(LED_G, color.green);
  analogWrite(LED_B, color.blue);
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  color = CRGB::Black;
  setLED(color);

  FastLED.addLeds<LED_TYPE,PIN_STRIP_DATA,PIN_STRIP_CLK,STRIP_COLOR_ORDER>(strip, STRIP_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.setMaxRefreshRate(FPS);

  Fire__init(heat, STRIP_PIXEL_COUNT);
  cooling = 25;
  sparking = 85;
  base = 4;

  step = 0;

  pinMode(PIN_DIRT, INPUT);
}

void animationInit() {
  chan = random(3);
  dir  = color[chan] <= MIN ? 1 : -1;
  step = color[chan];
  chill = random(CHILL) + CHILL/4;
}

void animationStep() {
  color[chan] = step;
  chill -= 1;

  Fire__eachStep(heat, STRIP_PIXEL_COUNT, cooling, sparking, base);
  for (int i; i<STRIP_PIXEL_COUNT; i++) {
    strip[i] = HeatColor(heat[i]);
  }
}

void animationNext() {
  step = step + dir;
  delay(1000/FPS);
}

void readDirt() {
  int dirt = analogRead(PIN_DIRT);
#ifdef DEBUG
  Serial.print("Dirt: ");
  Serial.println(dirt);
#endif
}

void loop() {
  FastLED.show();
  setLED(color);
  if ( (step <= MIN) || (step >= MAX) || (chill == 0) )  { // Start of animation
    animationInit();
  }
  animationNext();
  animationStep();

  EVERY_N_SECONDS(1) {
    readDirt();
  }
}

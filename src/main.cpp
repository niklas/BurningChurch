#include <Arduino.h>
#include <FastLED.h>

#define LED_R 6
#define LED_G 5
#define LED_B 3

#define FPS 30
#define MIN 2
#define MAX 123
#define CHILL 42

CRGB color;

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
  color = CRGB::Black;
  setLED(color);

  step = 0;
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
}

void animationNext() {
  step = step + dir;
  delay(1000/FPS);
}

void loop() {
  setLED(color);
  if ( (step <= MIN) || (step >= MAX) || (chill == 0) )  { // Start of animation
    animationInit();
  }
  animationNext();
  animationStep();
}

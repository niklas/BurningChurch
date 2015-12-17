#include <Arduino.h>
#include <FastLED.h>

#define LED_R 6
#define LED_G 5
#define LED_B 3
#define FPS 180

#define MAX 120

CRGB color;

int step;

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

byte chan;
int dir;

void animationInit() {
  chan = random(3);
  dir  = color[chan] == 0 ? 1 : -1;
  step = color[chan] == 0 ? 0 : MAX;
}

void animationStep() {
  color[chan] = step;
}

void animationNext() {
  step = step + dir;
  delay(1000/FPS);
}

void loop() {
  setLED(color);
  if ( (step <= 0) || (step >= MAX))  { // Start of animation
    animationInit();
  }
  animationNext();
  animationStep();
}

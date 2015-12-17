#include <Arduino.h>

#define LED_R 3
#define LED_G 5
#define LED_B 6

void setup() {
  analogWrite(LED_R, 0);
  analogWrite(LED_G, 0);
  analogWrite(LED_B, 0);
}

void loop() {
  byte r,g,b;
  r = random(255);
  g = random(255);
  b = random(255);
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);

  delay(1000);
}

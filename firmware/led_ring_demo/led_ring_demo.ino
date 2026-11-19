// Reference example for two 8-pixel WS2812B rings; not the exact archived test sketch.
#include <Adafruit_NeoPixel.h>
constexpr uint8_t LEDS = 8;
Adafruit_NeoPixel ring1(LEDS, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring2(LEDS, 5, NEO_GRB + NEO_KHZ800);
void showBoth(uint32_t c) {
  ring1.fill(c); ring2.fill(c); ring1.show(); ring2.show();
}
void setup() {
  Serial.begin(115200);
  ring1.begin(); ring2.begin();
  ring1.setBrightness(35); ring2.setBrightness(35);
  ring1.clear(); ring2.clear(); ring1.show(); ring2.show();
}
void loop() {
  showBoth(ring1.Color(255, 0, 0)); delay(800);
  showBoth(ring1.Color(0, 255, 0)); delay(800);
  showBoth(ring1.Color(0, 0, 255)); delay(800);
  showBoth(ring1.Color(255, 255, 255)); delay(800);
  showBoth(0); delay(300);
}

#include <FastLED.h>

// Adapted from https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    5
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    50
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          64 /* 96 */
#define FRAMES_PER_SECOND  30 /* 120 */

void setup() {
  delay(3000); // 3 second delay for recovery

  // limit my draw to 1A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5,1000); 
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
  tree,
  cloudColors,
//  oceanColors,
  partyColors,
//  customPalette,
//  rainbow,
  rainbowWithGlitter,
//  confetti,
//  sinelon,
//  juggle,
//  bpm
};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(20) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) 
{
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void tree() {
  fill_solid(leds, NUM_LEDS, CRGB::Green);

  leds[NUM_LEDS-1] = CRGB::Yellow;
  leds[NUM_LEDS-2] = CRGB::Yellow;
  leds[NUM_LEDS-3] = CRGB::Yellow;

  if (random8() < 40) {
    leds[random16(NUM_LEDS)] += CRGB::Grey;
  }
}

void oceanColors()
{
  setPalette(OceanColors_p, LINEARBLEND);
}

void cloudColors()
{
  setPalette(CloudColors_p, LINEARBLEND);
}

void partyColors()
{
  setPalette(PartyColors_p, LINEARBLEND);
}

void customPalette()
{
  const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
  {
      CRGB::Red,
      CRGB::Gray, // 'white' is too bright compared to red and blue
      CRGB::Blue,
      CRGB::Black,
      
      CRGB::Red,
      CRGB::Gray,
      CRGB::Blue,
      CRGB::Black,
      
      CRGB::Red,
      CRGB::Red,
      CRGB::Gray,
      CRGB::Gray,
      CRGB::Blue,
      CRGB::Blue,
      CRGB::Black,
      CRGB::Black
  };
  setPalette(myRedWhiteBluePalette_p, LINEARBLEND);
}

void setPalette(CRGBPalette16 palette, TBlendType blending) {
  uint8_t colorIndex = gHue;
  uint8_t brightness = 255;
    
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(palette, colorIndex, brightness, blending);
    colorIndex += 3;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS-1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

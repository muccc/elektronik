
#include "ws2812.h"

#define PIN 13    // pin where the LED data is connected
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))    // Structure sife for WS2812 LEDs
const uint16_t NUM_PIXELS = 16;  // <--- modify to suit your configuration
rgbVal pixels[NUM_PIXELS];    // Array to collect the LED color


// sensors
int version = 1;
unsigned int luftqulitaet=0;

struct RGB                                                                                                                       
{
  unsigned char R;
  unsigned char G;
  unsigned char B;
};

void setup() {
  Serial.begin(115200);
  initLEDs();
  pinMode(A0, INPUT);
}

// the mail loop
void loop() {
  Measure();
  delay(1000);      // wait for 1 second
}

void Measure(){
  luftqulitaet = (unsigned int) analogRead(A6);
  DisplayLED2();
}

void DisplayLED1(){
  int ledPos = 0;
  rgbVal ledColor = makeRGBVal(30, 0, 00); // Red
  if (luftqulitaet > 350) {
    ledColor = makeRGBVal(30, 30, 0); // Yellow
  } else if (luftqulitaet > 180) {
    ledColor = makeRGBVal(0, 30, 0); // Green
  } else {
    ledColor = makeRGBVal(0, 0, 30); // Blue
  }
  while (ledPos <  NUM_PIXELS) {
    pixels[ledPos++] = ledColor;
  }
  ws2812_setColors(COUNT_OF(pixels), pixels);
}

void DisplayLED2(){
  struct RGB myRGB;  
  int ledPos = 0;
  Serial.println((luftqulitaet/3) % 360);
  while (ledPos <  NUM_PIXELS) {
    myRGB = HSLToRGB((luftqulitaet/3) % 360,0.8,0.2);
    pixels[ledPos++] = makeRGBVal(myRGB.R, myRGB.G, myRGB.B);
  }
  ws2812_setColors(COUNT_OF(pixels), pixels);
}



void initLEDs(){
    // set all LEDs to dark
    ws2812_init(PIN);
     for (int i = 0; i < COUNT_OF(pixels); i++) {
      pixels[i] = makeRGBVal(0, 0, 0);  // set all LEDs to dark
    }
}


struct RGB HSLToRGB(  int H, float S, float L) {
  struct RGB rgb;
  if (S == 0)
  {
    rgb.R = rgb.G = rgb.B = (unsigned char)(L * 255);
  }
  else
  {
    float v1, v2;
    float hue = (float)H / 360;
    v2 = (L < 0.5) ? (L * (1 + S)) : ((L + S) - (L * S));
    v1 = 2 * L - v2;
    rgb.R = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
    rgb.G = (unsigned char)(255 * HueToRGB(v1, v2, hue));
    rgb.B = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
  }
  return rgb;
}


float HueToRGB(float v1, float v2, float vH)
{
  if (vH < 0)
    vH += 1;

  if (vH > 1)
    vH -= 1;

  if ((6 * vH) < 1)
    return (v1 + (v2 - v1) * 6 * vH);

  if ((2 * vH) < 1)
    return v2;

  if ((3 * vH) < 2)
    return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

  return v1;
}







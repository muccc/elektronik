
#include "ws2812.h"
#include <WiFi.h>
#include "esp_system.h"

#define PIN 13    // pin where the LED data is connected
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))    // Structure sife for WS2812 LEDs
const uint16_t NUM_PIXELS = 16;  // <--- modify to suit your configuration
rgbVal pixels[NUM_PIXELS];    // Array to collect the LED color
rgbVal HighColor = makeRGBVal(80, 0, 0);  // red LED
rgbVal LowColor = makeRGBVal(0, 0, 70);   // blue LED
int ledSequence[12] = { 1, 2, 6, 7, 15, 14, 13, 12, 11, 10, 4, 3 };
const char* ssid     = "muccc.legacy-2.4GHz";
const char* password = "haileris";
const char* host = "google.com";

int interation = 0;                       // a counter to get the internet time only a a certain time; the interwal will increase every second
int updateInterval = 120;                  // after how many intervals the time will be updated
int h = 0;          // hour
int m = 0;          // minute
int s = 0;          // second

// sensors
uint8_t temprature_sens_read(); 
int version = 1;

struct RGB
{
  unsigned char R;
  unsigned char G;
  unsigned char B;
};

void setup() {
  Serial.begin(115200);
  delay(10);
  initLEDs();
  connectWIFI();
}

// the mail loop
void loop() {
  interation++;
  if (interation % updateInterval == 0) {
    updateTime();   // get internet time from while to while
  } 
  s++;              // count the next second
  validateTime();   // check time format
  //updateBinaryClock();      // Show the tie as binary LED display
  LEDAnimation();
  delay(1000);      // wait for 1 second
}

void LEDAnimation(){
  //Serial.println(temprature_sens_read());
  pinMode(A0, INPUT);
  Serial.print("Sensor: ");
  Serial.println(analogRead(A0));
  struct RGB myRGB;  
  
  int ledPos = 0;
  while (ledPos <  sizeof(ledSequence)/ sizeof(int)) {
    myRGB = HSLToRGB((interation + (360/ sizeof(ledSequence)/ sizeof(int))*ledPos) % 360,0.8,0.2);
    pixels[ledSequence[ledPos++]] = makeRGBVal(myRGB.R, myRGB.G, myRGB.B);
  }
  ws2812_setColors(COUNT_OF(pixels), pixels);
}

void updateBinaryClock(){

  // Set the diffenert LEDs binary of the time
  pixels[15] = ((h & 8) == 8) ? HighColor : LowColor;
  pixels[14] = ((h & 4) == 4) ? HighColor : LowColor;
  pixels[13] = ((h & 2) == 2) ? HighColor : LowColor;
  pixels[12] = ((h & 1) == 1) ? HighColor : LowColor;
  pixels[11] = ((m & 32) == 32) ? HighColor : LowColor;
  pixels[10] = ((m & 16) == 16) ? HighColor : LowColor;
  pixels[9] = ((m & 8) == 8) ? HighColor : LowColor;
  pixels[8] = ((m & 4) == 4) ? HighColor : LowColor;
  pixels[7] = ((m & 2) == 2) ? HighColor : LowColor;
  pixels[6] = ((m & 1) == 1) ? HighColor : LowColor;
  pixels[5] = ((s & 32) == 32) ? HighColor : LowColor;
  pixels[4] = ((s & 16) == 16) ? HighColor : LowColor;
  pixels[3] = ((s & 8) == 8) ? HighColor : LowColor;
  pixels[2] = ((s & 4) == 4) ? HighColor : LowColor;
  pixels[1] = ((s & 2) == 2) ? HighColor : LowColor;
  pixels[0] = ((s & 1) == 1) ? HighColor : LowColor;
  //Serial.println(String(h) + ":" + String(m) + ":" + String(s));
  ws2812_setColors(COUNT_OF(pixels), pixels);

}

void initLEDs(){
    ws2812_init(PIN);
     for (int i = 0; i < COUNT_OF(pixels); i++) {
      pixels[i] = makeRGBVal(0, 0, 0);  // set all LEDs to dark
    }
}

void connectWIFI(){
      // We start by connecting to a WiFi network0
    Serial.print("Version:");
    Serial.println(version);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void updateTime(){
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/input/";
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and extract the time
    while(client.available()) {
      String line = client.readStringUntil('\n');
      line.toUpperCase();
      // example: 
      // date: Thu, 19 Nov 2015 20:25:40 GMT
      if (line.startsWith("DATE: ")) {
        //Serial.println(line.substring(23, 25) + ":" + line.substring(26, 28) + ":" +line.substring(29, 31));
        h = line.substring(23, 25).toInt();     // extract hour
        m = line.substring(26, 28).toInt();     // extract minute
        s = line.substring(29, 31).toInt();     // extract second
        h = h+1;                                // Convert from GMT to MEZ
        validateTime();
        Serial.println(String(h) + ":" + String(m) + ":" + String(s)); 
      }
    }
}

void validateTime(){
  // Corrects overflof of incrementing seconds, minutes, hours
  if (s >= 60) {
    s = 0;
    m = m +1;
    if (m >= 60) {
      m = 0;
      h = h +1;
      if (h >= 24){
        h = 0;
      }
    }
 }
 // convert for 12h and NOT 24h format.
 if (h > 12) {
    h = h - 12;
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







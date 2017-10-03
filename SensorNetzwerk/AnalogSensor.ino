#include <Wire.h>             // Needed for legacy versions of Arduino.
#include <BME280I2C.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#define PIN D2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
SSD1306  display(0x3c, D3, D4);
float temp(NAN), hum(NAN), pres(NAN), qual(NAN);
BME280I2C bme;                   // Default : forced mode, standby time = 1000 ms
bool metric = true;
void printBME280Data(Stream * client);
void printBME280CalculatedData(Stream* client);
int val = 1;                    //Startzustand ausgeschaltet
String Temp = "";
char stringTemp[8];
WiFiClient webClient;
const byte BH1750FVI_I2CADDRESS = 0x23;  // I2C address (ADD=L=0100011)
byte buffer[2];                                                      // Array to store 2 bytes of light value
unsigned int lightSensor = 0; // Max. sensorValue allowed up to (2^16)-1


#define SERIAL_BAUD 115200
const char* ssid = "muccc.legacy-2.4GHz";
const char* password = "haileris";

// ThingSpeak Connection to store the data
String apiKey = "5ZE48RYVYV6P8EHW";
const char* dataServer = "api.thingspeak.com";





void reconnect() {
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    displayIP();
  }
}



void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {} // Wait
  setupDisplay();
  delay(10);
  Serial.println("Hello Setup");
  delay(100);
  Wire.begin(D3, D4);
  while (!bme.begin()) {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  BH1750FVIinit(BH1750FVI_I2CADDRESS);
  Serial.println(WiFi.status());
  WiFi.begin(ssid, password);
  Serial.println(WiFi.status());
  reconnect();
  Serial.println("Sensor active");
}



void loop() {
  delay(15000);
  qual = 100 - float(analogRead(A0)) / 10;
  printBME280Data(&Serial);
  printBME280CalculatedData(&Serial);
  measureLight();
  transferWebClinet();
  displayWeather();
  if (qual > 93) {
    strip.setPixelColor(0, strip.Color(0, 0, 255));
  } else if (qual > 90) {
    strip.setPixelColor(0, strip.Color(0, 255, 0));
  } else if (qual > 85) {
    strip.setPixelColor(0, strip.Color(255, 255, 0));
  } else {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
  }
  strip.show(); // Initialize all pixels to 'off'
}

void measureLight() {
  delay(100);
  if (BH1750read(BH1750FVI_I2CADDRESS) == 2)
  {
    lightSensor = ((buffer[0] << 8) | buffer[1]);       // Combine 2 bytes
    //Serial.print("Beleuchtungsstaerke: ");
    //Serial.print(lightSensor);
    //Serial.println("lx");
  }
}

void BH1750FVIinit(byte I2Caddress)
{
  Wire.beginTransmission(I2Caddress);             // I2C address
  Wire.write(0x10);                                              // Switch to H-Mode 1Lux resolution 120ms
  //Wire.write(0x11);                                            // Switch to H-Mode2 0,5Lux resolution
  //Wire.write(0x13);                                            // Switch to L-Mode 4 Lux resolution 16ms
  Wire.endTransmission();
}

int BH1750read(int I2Caddress)
{
  byte byteNumber = 0;
  Wire.beginTransmission(I2Caddress);            // I2C address
  Wire.requestFrom(I2Caddress, 2);                  // Request fromm I2Caddress 2 bytes
  while (Wire.available())
  {
    buffer[byteNumber] = Wire.read();                   // Read 2 bytes into array
    byteNumber++;
  }
  Wire.endTransmission();
  return byteNumber;
}

void setupDisplay() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Init");
}

void displayWeather() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Temp");
  dtostrf(temp, 1, 1, stringTemp);
  display.drawString(64, 0, stringTemp);
  display.drawString(0, 14, "Humidy");
  dtostrf(hum, 1, 1, stringTemp);
  display.drawString(64, 14, stringTemp);
  display.drawString(0, 28, "Preasue");
  dtostrf(pres, 1, 1, stringTemp);
  display.drawString(64, 28, stringTemp);
  display.drawString(0, 42, "Quality");
  dtostrf(qual, 1, 1, stringTemp);
  display.drawString(64, 42, stringTemp);
  display.display();
}

void displayIP() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connected");
  display.drawString(0, 14, ssid);
  display.drawString(0, 28, WiFi.localIP().toString());
  display.setFont(ArialMT_Plain_10);
  display.display();
}


void transferWebClinet() {
  if (webClient.connect(dataServer, 80)) { // "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
    postStr += "&field1=";
    dtostrf(temp, 1, 5, stringTemp);
    postStr += String(stringTemp);
    postStr += "&field2=";
    dtostrf(hum, 1, 5, stringTemp);
    postStr += String(stringTemp);
    postStr += "&field3=";
    dtostrf(pres, 1, 5, stringTemp);
    postStr += String(stringTemp);
    postStr += "&field4=";
    dtostrf(qual, 1, 5, stringTemp);
    postStr += String(stringTemp);
    postStr += "&field5=";
    postStr += String(lightSensor);
    postStr += "\r\n\r\n";


    webClient.print("POST /update HTTP/1.1\n");
    webClient.print("Host: api.thingspeak.com\n");
    webClient.print("Connection: close\n");
    webClient.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    webClient.print("Content-Type: application/x-www-form-urlencoded\n");
    webClient.print("Content-Length: ");
    webClient.print(postStr.length());
    webClient.print("\n\n");
    webClient.print(postStr);
  }
  webClient.stop();
}





/* ==== Functions ==== */
void printBME280Data(Stream* client) {
  uint8_t pressureUnit(1);                                           // unit: B000 = Pa, B001 = hPa, B010 = Hg, B011 = atm, B100 = bar, B101 = torr, B110 = N/m^2, B111 = psi
  bme.read(pres, temp, hum, metric, pressureUnit);                   // Parameters: (float& pressure, float& temp, float& humidity, bool celsius = false, uint8_t pressureUnit = 0x0)
  client->print("Qual: ");
  client->print(qual);
  client->print("\tTemp: ");
  client->print(temp);
  client->print("°" + String(metric ? 'C' : 'F'));
  client->print("\tHumidity: ");
  client->print(hum);
  client->print("% RH");
  client->print("\tPressure: ");
  client->print(pres);
  client->print(" atm");
  client->print("\tLight: ");
  client->print(lightSensor);
  client->print(" lx");
}

void printBME280CalculatedData(Stream* client) {
  float altitude = bme.alt(metric);
  float dewPoint = bme.dew(metric);
  client->print("\tAltitude: ");
  client->print(altitude);
  client->print((metric ? "m" : "ft"));
  client->print("\tDew point: ");
  client->print(dewPoint);
  client->println("°" + String(metric ? 'C' : 'F'));
}






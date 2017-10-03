#include <Adafruit_NeoPixel.h>
#define LEDPIN D2
#define RELAIS D6
#define SERIAL_BAUD 115200
int MINUTES=240;
int SECONDS=60;
int WATERING=60000;
int HUMIDITY=15;
float qual=0;
float qualavg=0;
int countDown=0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, LEDPIN, NEO_GRB + NEO_KHZ800);




void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  pinMode(RELAIS, OUTPUT);
  digitalWrite(RELAIS,LOW);
  Serial.begin(SERIAL_BAUD);
  strip.begin();
  while (!Serial) {} // Wait
  qualavg = 100 - float(analogRead(A0)) / 10;
  delay(100);
  Serial.println("Sensor active");
}

void loop() {
  digitalWrite(RELAIS,LOW);
  delay(100);
  // m = 60 = 1 Stunde
  for (int m = 0; m < MINUTES; m++) {
    // i = 60 = 1 Minute
    for (int i = 0; i < SECONDS; i++) {
      delay(1000);
      qual = 100 - float(analogRead(A0)) / 10;
      qualavg = (qualavg * 29 + qual)/30;
      countDown = (MINUTES*SECONDS)-(i+(SECONDS*m));
      Serial.print(countDown);
      Serial.print("  ");
      Serial.print(qual);
      Serial.print("  ");
      Serial.println(qualavg);
      if (qualavg > 40) {
        strip.setPixelColor(0, strip.Color(0, 0, 100));
      } else if (qualavg > HUMIDITY) {
        strip.setPixelColor(0, strip.Color(0, 50, 0));
      } else if (qualavg > 3.8) {
        strip.setPixelColor(0, strip.Color(150, 100, 0));
      } else {
        strip.setPixelColor(0, strip.Color(200, 0, 0));
      }
      strip.show(); 
    }
  }
  // wenn Feuchtigkeit kleiner als 10, dann bewässern
  // wenn Feuchtigkeit kleiner als 3,5 dann nicht Pumpen weil Sensorfehler
  if (qualavg < HUMIDITY && qualavg > 3.5) {
    Serial.println("Pumpe ON");
    strip.setPixelColor(1, strip.Color(0, 150, 100));
    strip.show(); // Initialize all pixels to 'off'
    digitalWrite(RELAIS,HIGH);
    //  Pumpen
    delay(WATERING);
    strip.setPixelColor(1, strip.Color(0, 0, 0));
    strip.show(); // Initialize all pixels to 'off'
    
  }
  Serial.println("Pumpe OFF");
  digitalWrite(RELAIS,LOW);
}







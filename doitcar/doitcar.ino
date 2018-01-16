
/*
 * This sketch lets you control an NodeMCU motorshield with attached engines
 * from a (smart phone) browser by using the orientation of the device.
 */

#include <ESP8266WiFi.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
#define GYRO_ACTIVE
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;


const char* ssid = "muccc.legacy-2.4GHz";
const char* password = "haileris";
int left = 0;
int right = 0;
String lastReq="";
WiFiClient client;
#define TRIGGER 14
#define ECHO    12

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);
//reads distance from an HC-SR04 sensor. Currently defined on GPIO 14 for TRIGGER
//and ECHO on GPIO 12

#if 0
void handleMotor_manual(int a, int b){
  DBG_OUTPUT_PORT.printf("A:%d, B:%d\n",a,b);

  analogWrite(5, abs(a));      // was left
  analogWrite(4, abs(a));
  digitalWrite(0, motorBForward); // was A
  digitalWrite(2, motorAForward); // was B

}
#endif
long read_distance(){
  long duration, distance;
  digitalWrite(TRIGGER, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGGER, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  return distance;
}
void initGyro(){
    Wire.begin(D5,D6);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values

    Serial.println("Updating internal sensor offsets...");
    // -76  -2359 1688  0 0 0
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    accelgyro.setXAccelOffset(0);
    accelgyro.setYAccelOffset(0);
    accelgyro.setZAccelOffset(0);
    accelgyro.setXGyroOffset(1000);
    accelgyro.setYGyroOffset(1000);
    accelgyro.setZGyroOffset(0);
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
}

void setup() {
  Serial.begin(115200);
  delay(10);
  #ifdef GYRO_ACTIVE initGyro();
  #endif

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(5, 0);
  digitalWrite(4, 0);

  digitalWrite(0, 1);
  digitalWrite(2, 1);

}

void readGyro(){
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

         Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
}

void readGyroHTML(){
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    client.print("<table>");
    client.print("<tr><td>ax:</td><td>");
    client.print(ax);
    client.print("</td></tr>");
    client.print("<tr><td>ay:</td><td>");
    client.print(ay);
    client.print("</td></tr>");
    client.print("<tr><td>az:</td><td>");
    client.print(az);
    client.print("</td></tr>");
    client.print("<tr><td>gx:</td><td>");
    client.print(gx);
    client.print("</td></tr>");
    client.print("<tr><td>gy:</td><td>");
    client.print(gy);
    client.print("</td></tr>");
    client.print("<tr><td>gz:</td><td>");
    client.print(gz);
    client.print("</td></tr>");
    client.print("<table><BR/><BR/>");

}

void loop() {
  // Check if a client has connected
  client = server.available();
  if (!client) {
    analogWrite(5, 0);
    analogWrite(4, 0);
    digitalWrite(0, 1);
    digitalWrite(2, 1);
    return;
  }

  // Wait until the client sends some data
  //Serial.println("new client");
  while(!client.available()){
    delay(20);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');

  client.flush();
  if (lastReq.equals(req)) {
    // the same command not again
    yield();
    return;
  }
  Serial.println(req);
  // Match the request
  int motorASpeed = 1023;
  int motorBSpeed = 1023;
  int motorAForward = 1;
  int motorBForward = 1;
  if (req.indexOf("/engines/") != -1) {
    String parameters = req.substring(13);
    int separatorPos = parameters.indexOf(",");
    int httpPos = parameters.indexOf(" HTTP");
    String leftText = parameters.substring(0,separatorPos);
    String rightText = parameters.substring(separatorPos + 1, httpPos);

    Serial.println("[" + leftText +"][" + rightText + "]");
    left = leftText.toInt();
    right = rightText.toInt();
    if (left < 0) {
      motorAForward = 0;
    } else {
      motorAForward = 1;
    }
    if (right < 0) {
      motorBForward = 0;
    } else {
      motorBForward = 1;
    }
    analogWrite(5, abs(left));
    analogWrite(4, abs(right));
    digitalWrite(0, motorAForward);
    digitalWrite(2, motorBForward);
    yield();
    #ifdef GYRO_ACTIVE readGyro();
    #endif
  } else if (req.indexOf("/index.html") != - 1 || req.indexOf("/") != - 1) {
    Serial.println("index.html");
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n");
    client.print("<html><head>");
    client.print("</head><body><font size=\"6\">");
    client.print("<script type='text/javascript' src='http://www.squix.org/blog/smartcar.js'></script>");
    client.print("<a href='#' onclick='move(\"f\");'>forward</a><BR/>");
    client.print("<a href='#' onclick='move(\"b\");'>backwards</a><BR/>");
    client.print("<a href='#' onclick='move(\"l\");'>left</a><BR/>");
    client.print("<a href='#' onclick='move(\"r\");'>right</a><BR/>");
    #ifdef readGyroHTML();
    #endif
    client.print("<div id=\"dmEvent\"/>");
    client.print("<div id=\"vector\"/>");
	client.print("<div id=\"EPSStatus\"/>");
    client.print("</font></body></html>");
    analogWrite(5, 0);
    analogWrite(4, 0);
    digitalWrite(0, 1);
    digitalWrite(2, 1);
    yield();
    return;
  }


  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);

  yield();
  delay(50);
  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

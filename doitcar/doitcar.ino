/*
 * This sketch lets you control an NodeMCU motorshield with attached engines
 * from a (smart phone) browser by using the orientation of the device.
 */

#include <ESP8266WiFi.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

const char* ssid = "muccc.legacy-2.4GHz";
const char* password = "haileris";


class Motor {
    public:
        Motor(int gpio_d, int gpio_a, bool reverse);
        void update(void);
        void setSpeed(int speed);

    private:
        int speed = 0;
        int gpio_d;
        int gpio_a;
        unsigned long int lastUpdate = 0;
        unsigned long int lastApplied = 0;
        bool reverse;
};

class WebServer {
    public:
        WebServer(Motor *motorLeft, Motor *motorRight);
        ~WebServer(void);
        void update(void);

    private:
        void printIndex(WiFiClient* client);
        void engine(String req);
        WiFiServer *server;
        Motor *motorLeft;
        Motor *motorRight;
};

WebServer::WebServer(Motor *motorLeft, Motor *motorRight)
{
    this->motorLeft = motorLeft;
    this->motorRight = motorRight;

    server = new WiFiServer(80);
    server->begin();
}

WebServer::~WebServer(void)
{
    server->stop();
    delete server;
}

void WebServer::printIndex(WiFiClient *client)
{
    client->print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n");
    client->print("<html><head>");
    client->print("</head><body><font size=\"6\">");
    client->print("<script type='text/javascript' src='http://www.squix.org/blog/smartcar.js'></script>");
    client->print("<a href='#' onclick='move(\"f\");'>forward</a><BR/>");
    client->print("<a href='#' onclick='move(\"b\");'>backwards</a><BR/>");
    client->print("<a href='#' onclick='move(\"l\");'>left</a><BR/>");
    client->print("<a href='#' onclick='move(\"r\");'>right</a><BR/>");
    client->print("<div id=\"dmEvent\"/>");
    client->print("<div id=\"vector\"/>");
    client->print("<div id=\"EPSStatus\"/>");
    client->print("</font></body></html>");
}

void WebServer::engine(String req)
{
    String parameters = req.substring(13);
    int separatorPos = parameters.indexOf(",");
    int httpPos = parameters.indexOf(" HTTP");
    String leftText = parameters.substring(0, separatorPos);
    String rightText = parameters.substring(separatorPos + 1, httpPos);

    Serial.println("[" + leftText +"][" + rightText + "]");

    motorLeft->setSpeed(leftText.toInt());
    motorRight->setSpeed(rightText.toInt());
}

void WebServer::update(void)
{
    WiFiClient client = server->available();

    if (!client.available())
        return;

    // Read the first line of the request
    String req = client.readStringUntil('\r');

    client.flush();

    if (req.indexOf("/index.html ") != - 1 || req.indexOf("/ ") != - 1) {
        printIndex(&client);
    } else if (req.indexOf("/engines/") != -1) {
        engine(req);
    }
}


Motor::Motor(int gpio_d, int gpio_a, bool reverse)
{
    this->gpio_d = gpio_d;
    this->gpio_a = gpio_a;
    this->reverse = reverse;

    pinMode(gpio_d, OUTPUT);
    pinMode(gpio_a, OUTPUT);
    digitalWrite(gpio_d, HIGH);
    digitalWrite(gpio_a, LOW);
}

void Motor::setSpeed(int speed)
{
    this->speed = speed;
    lastUpdate = millis();
}

void Motor::update(void)
{
    unsigned long now = millis();

    if (lastUpdate > lastApplied) {
        if (speed < 0) {
            digitalWrite(gpio_d, reverse ? HIGH : LOW);
        } else {
            digitalWrite(gpio_d, reverse ? LOW : HIGH);
        }
        analogWrite(gpio_a, abs(speed));
        lastApplied = now;
    } else if (lastApplied + 500 <  now) {
        analogWrite(gpio_a, 0);
    }
}

#if 0
void handleMotor_manual(int a, int b){
  DBG_OUTPUT_PORT.printf("A:%d, B:%d\n",a,b);

  analogWrite(5, abs(a));      // was left
  analogWrite(4, abs(a));
  digitalWrite(0, motorBForward); // was A
  digitalWrite(2, motorAForward); // was B

}
#endif
#if 0
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
#endif

#if 0
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
#endif

#if 0
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
#endif

#if 0
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
#endif


Motor *motorLeft;
Motor *motorRight;
WebServer *web;

void setup() {
    Serial.begin(115200);
    delay(10);

    // Connect to WiFi network
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

    // Print the IP address
    Serial.println(WiFi.localIP());

    motorLeft = new Motor(0, 5, true);
    motorRight = new Motor(2, 4, false);
    web = new WebServer(motorLeft, motorRight);

    Serial.println("Server started");
}

void loop() {
    motorLeft->update();
    motorRight->update();
    web->update();
}
